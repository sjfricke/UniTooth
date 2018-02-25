#ifndef _UNITOOTH__H
#define _UNITOOTH__H

typedef void (*callbackStr)(char*);

// RFCOMM
int serverR();

int sendR(char *msg);

void set_callbackR(callbackStr callback);

// L2CAP
int serverL();

int sendL(char *msg);

void set_callbackL(callbackStr callback);

#endif
