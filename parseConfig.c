#include "parseConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#define MAXBUF 1024
#define DELIM "="
#define FILENAME "/qemu/.config.conf"

struct config get_config()
{
        struct config conf;
        FILE *file = fopen (FILENAME, "r");
        char *token;
        if (file != NULL)
        {
                char line[MAXBUF];
                while(fgets(line, sizeof(line), file) != NULL)
                {
                  int len = strlen(line);
                  if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
                        token = strtok(line, DELIM);
                        if (token != NULL) {
                              if(strcmp(token,"server_addr") ==0)
                              {
                                token = strtok(NULL,DELIM);
                                if(token !=NULL){
                                  strcpy(conf.server_addr,token);
                                }else{
                                  syslog(LOG_ERR,"Error reading server_addr value from config \n" );
                                }
                              }else if(strcmp(token,"port") ==0){
                                token = strtok(NULL,DELIM);
                                if(token !=NULL){
                                  conf.port = atoi(token);
                                }else{
                                  syslog(LOG_ERR,"Error reading value from config \n" );
                                }
                              }else if(strcmp(token,"temperature_file") ==0){
                                token = strtok(NULL,DELIM);
                                if(token !=NULL){
                                  strcpy(conf.temperature_file,token);
                                }else{
                                  syslog(LOG_ERR,"Error reading temperature_file value from config \n" );
                                }
                              }else if(strcmp(token,"status_file") ==0){
                                token = strtok(NULL,DELIM);
                                if(token !=NULL){
                                  strcpy(conf.status_file,token);
                                }else{
                                  syslog(LOG_ERR,"Error reading status_file value from config \n" );
                                }
                              }else if(strcmp(token,"serv_sync_freq_sec") ==0){
                                token = strtok(NULL,DELIM);
                                if(token !=NULL){
                                  conf.serv_sync_freq_sec=atoi(token);
                                }else{
                                  syslog(LOG_ERR,"Error reading serv_sync_freq_sec value from config \n" );
                                }
                              }else{
                                syslog(LOG_ERR,"Invalid config value%s\n",token );
                              }
                          }
                } // End while
                fclose(file);
        } // End if file
        else{
          syslog(LOG_ERR,"Failed to open file %s\n",FILENAME );
        }
        return conf;
}
