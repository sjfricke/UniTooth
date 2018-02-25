#ifndef _UNITOOTH__H
#define _UNITOOTH__H

typedef void (*callbackStr)(char*);

// RFCOMM
int serverR();

void *server_daemonR();

int sendR(char *msg);

void *recv_handleR();

void set_callbackR(callbackStr callback);

// L2CAP
int serverL();

void *server_daemonL();

int sendL(char *msg);

void *recv_handleL();

void set_callbackL(callbackStr callback);

#endif
