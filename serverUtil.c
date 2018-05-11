#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "serverUtil.h"
#include "parseConfig.h"

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
  printf("Status Config ADDRESS %s\n",app_conf.server_addr);

  char url[255];
  sprintf(url, "%s:%d/status",app_conf.server_addr, app_conf.port);
  printf("Updated URL %s\n",url );
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
      printf("Curl error!\n");
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
      ret_code = 1;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code != 200)
        ret_code =-1;
    //printf("%ld %s\n", http_code, response);
    curl_easy_cleanup(curl);
    curl_slist_free_all(slist);
  }
  return ret_code;
}

int doUpdate(double temperature, char status[]) {
  int ret = updateStatus(250, "Off");
  if(ret==0 && strcmp(response,"OK") ==0)
  {
    printf("Sucess\n" );
  }else{
    printf("Failed%s\n",response );
  }
  return 0;
}
int doGetSetpoints(){
  printf("setpoints Config ADDRESS %s\n",app_conf.server_addr);

  CURL *curl;
  char setpointURL[255];
  sprintf(setpointURL, "%s:%d/setpointlist",app_conf.server_addr, app_conf.port);
  printf("URL   :%s\n",setpointURL );
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
      printf("Curl error!\n");
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      ret_code = 1;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code != 200)
        ret_code =-1;
    curl_easy_cleanup(curl);
    //curl_slist_free_all(slist);
  }

  return ret_code;
}
char * fetchSetpoints(){
    int ret =doGetSetpoints();
    if(ret==0)
    {
      printf("Sucess%s\n",response);
      return response;
    }else{
      printf("Failed%s\n",response );
    }
    return "FAIL";
}
