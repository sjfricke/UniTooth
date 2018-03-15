
#ifndef _UNITOOTH__H
#define _UNITOOTH__H

#include <errno.h>
#include <fcntl.h>

typedef void (*callbackStr)(char*);

// RFCOMM
int serverR();

int sendR(char *msg);

void set_callbackR(callbackStr callback);

// L2CAP
int serverL();

int sendL(char *msg);

void set_callbackL(callbackStr callback);

// Camera
int capture_init();
int capture_image();

void unitooth_close();

#endif
