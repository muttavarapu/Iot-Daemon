#ifndef __PARSECONFIG_H__
#define __PARSECONFIG_H__
#define MAXBUF 1024

struct config {
   int port;
   char server_addr[MAXBUF];
   char temperature_file[MAXBUF];
   char status_file[MAXBUF];
   int serv_sync_freq_sec;
};

struct config get_config();
struct config app_conf;


#endif
