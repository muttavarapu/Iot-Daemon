#ifndef __SERVERUTIL_H__
#define __SERVERUTIL_H__




int updateStatus(double temperature, char status[]);
int doUpdate(double temperature, char status[]);
char * fetchSetpoints();


#endif
