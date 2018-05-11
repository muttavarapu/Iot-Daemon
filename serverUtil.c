#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "serverUtil.h"
#include "parseConfig.h"
#include <syslog.h>

char response[512];
extern struct config app_conf;

char *buildStatusJSON(double temperature, char status[]) {
  char *jsonStr = malloc(256);
  sprintf(jsonStr, "{ \"temperature\":%2.13f,\"status\":\"%s\"}", temperature,
          status);
  return jsonStr;
}

static int writer(char *data, size_t size, size_t nmemb, char *buffer_in) {
  strcpy(response, data);
  return size * nmemb;
}

int updateStatus(double temperature, char status[]){
  char url[255];
  sprintf(url, "%s:%d/status",app_conf.server_addr, app_conf.port);
  char * inData = buildStatusJSON(250,"Off");
  CURL *curl;
  struct curl_slist *slist = NULL;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  int ret_code = 1;
  CURLcode res;
  if (curl) {
    ret_code =0;
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_URL,url );
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    slist = curl_slist_append(slist, "Accept: */*");
    // slist = curl_slist_append(slist, "Content-Type:
    // application/x-www-form-urlencoded");
    slist = curl_slist_append(slist, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inData);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (long)strlen(inData));

    char *dataPointer = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, dataPointer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
    syslog(LOG_ERR,"Curl error!\n");
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
      ret_code = 1;
    }else{

    	long http_code = 0;
    	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    	if(http_code != 200)
	        ret_code =-1;
	    //printf("%ld %s\n", http_code, response);
	}
    curl_easy_cleanup(curl);
    curl_slist_free_all(slist);
  }
  return ret_code;
}

int doUpdate(double temperature, char status[]) {
  int ret = updateStatus(250, "Off");
  if(ret!=0 && strcmp(response,"OK") !=0)
  {
    syslog(LOG_ERR,"Failed%s\n",response );
    return -1;
  }
  return 0;
}
int doGetSetpoints(){

  CURL *curl;
  char setpointURL[255];
  sprintf(setpointURL, "%s:%d/setpointlist",app_conf.server_addr, app_conf.port);

  //struct curl_slist *slist = NULL;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  int ret_code = 1;
  CURLcode res;
  if (curl) {
    ret_code =0;
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_URL, setpointURL);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    char *dataPointer = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, dataPointer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func_callback);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      syslog(LOG_ERR,"Curl error!\n");
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      ret_code = 1;
    }else{
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code != 200)
        ret_code =-1;
}
    curl_easy_cleanup(curl);
    //curl_slist_free_all(slist);
  }

  return ret_code;
}
char * fetchSetpoints(){
    int ret =doGetSetpoints();
    if(ret==0)
    {
      return response;
    }else{
      syslog(LOG_ERR,"Failed%s\n",response );
    }
    return "FAIL";
}
