#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>

#include "parseConfig.h"
#include "serverUtil.h"
#include "monitor_temp.h"

#define BUFF_SIZE 50
int secondsAgo = -1; // used to calculate update time
double currentTemp = 0.0;
double morning = 0.0;
double midDay = 0.0;
double evening = 0.0;
double night = 0.0;
extern struct config app_conf;
void heaterOn() {
  FILE *fptr = fopen(app_conf.status_file, "w");
  fprintf(fptr, "ON");
  fclose(fptr);
}

void heaterOff() {
  FILE *fptr = fopen(app_conf.status_file, "w");
  fprintf(fptr, "OFF");
  fclose(fptr);
}
char *getHeaterStatus() {
  char *heaterStatus = malloc(BUFF_SIZE);
  FILE *fptr = fopen(app_conf.status_file, "r");
  fgets(heaterStatus, BUFF_SIZE, fptr);
  fclose(fptr);
  return heaterStatus;
}
void readCurrentTemp() {
  FILE *fptr = fopen(app_conf.temperature_file, "r");
  if (fptr == NULL) {
    syslog(LOG_ERR,"Cannot open temperature file");
    exit(0);
  }
  fscanf(fptr, "%lf", &currentTemp);
  fclose(fptr);
}
double getSetPoint() {
  time_t now;
  struct tm *now_tm;
  int hour;
  now = time(NULL);
  now_tm = localtime(&now);
  hour = now_tm->tm_hour;
  if (hour > 11 && hour < 16) {
    return midDay;
  }else if (hour >= 16 && hour < 20) {
    return evening;
  }else if (hour >= 20 || hour < 4) {
    return night;
  }else{
      return morning;
  }
}

void syncWithServer() {
  if (secondsAgo > app_conf.serv_sync_freq_sec || secondsAgo == -1) {
    // update current temperature
    int ret = doUpdate(currentTemp, getHeaterStatus()); // serverUtil
    if (ret == 0) {
      secondsAgo = 0;
    } else {
      syslog(LOG_ERR,"Server update failed\n");
    }
    // get setpoints
    char *setPointsStr = fetchSetpoints(); // serverUtil
    if (strcmp(setPointsStr, "ERROR") != 0) {
      // get Comma seperated temperatures
      const char s[2] = ",";
      char *token;
      token = strtok(setPointsStr, s);
      if (token != NULL) {
        morning = atof(token);
      }
      token = strtok(NULL, s);
      if (token != NULL) {
        midDay = atof(token);
      }
      token = strtok(NULL, s);
      if (token != NULL) {
        evening = atof(token);
      }
      token = strtok(NULL, s);
      if (token != NULL) {
        night = atof(token);
      }
      syslog(LOG_INFO,"Set points- Mor:%lf,Aft:%lf,Ev:%lf,Ni:%lf \n",morning,midDay,evening,night);
    } else {
      secondsAgo = app_conf.serv_sync_freq_sec + 1;
      syslog(LOG_ERR,"Error getting setpoints from server! \n");
    }
  } else {
    secondsAgo++;
  }
}

void start_temp_monitor(){
  app_conf = get_config();
  int n;
  while (1) {
    readCurrentTemp();
    syncWithServer();
    double setPoint = getSetPoint();//based on time of the day and server settings
    if (currentTemp > setPoint) {
      syslog(LOG_INFO,"%lf: Too hot!. Turning off the heater\n", currentTemp);
      heaterOff();
    }
    if (currentTemp < setPoint) {
      syslog(LOG_INFO,"%lf: Too Cold!. Turning ON the heater\n", currentTemp);
      heaterOn();
    }
    sleep(1);
  }
}
