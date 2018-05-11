#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include "monitor_temp.h"

#define DAEMON_NAME "montempd"
#define OK          0
#define ERR_FORK   -1
#define ERR_SETSID -2
#define ERR_OPEN   -5
#define ERR_CLOSE  -6
#define ERR_WTF    -7

static void  _signal_handler(const int signal) {
  switch (signal) {
    case SIGHUP:
      break;
    case SIGTERM:
      syslog(LOG_INFO,"recieved SIGTERM,exiting");
      closelog();
      exit(OK);
      break;
    default:
      syslog(LOG_INFO,"recieved unhandled signal");
  }
  /* code */
}

int main(void){

  openlog(DAEMON_NAME,LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
  syslog(LOG_INFO,"Stattimg temp monitor daemon");

  pid_t pid =fork();
  if(pid <0){
    syslog(LOG_ERR,"ERR_FORMAT");
    return ERR_FORK;
  }

  if(pid >0){
    return OK;
  }
  if(setsid() < -1){
    syslog(LOG_ERR,"ERR_FORMAT");
    return ERR_SETSID;
  }
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  signal(SIGTERM,_signal_handler);
  signal(SIGHUP,_signal_handler);
  start_temp_monitor();
  return ERR_WTF;
}
