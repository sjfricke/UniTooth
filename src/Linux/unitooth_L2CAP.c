
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>  // socket, setsockopt, accept, send, recv
#include <sys/socket.h>

#include <signal.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include "unitooth.h"

pthread_t server_thread;
pthread_t rcv_handler_theread;

static int serv_sock; 		// server socket
static int client_connected = 0;// boolean to signify that client is connected
static int client = -1;		// client fd
static callbackStr onData = NULL;
static char recv_buf[1024];

static void *server_daemonL();
static void *recv_handlerL();

int serverL()
{
	return pthread_create(&server_thread,
			NULL,
			server_daemonL,
		       	NULL);
}


void *server_daemonL ()
{
	char command[64];

	// Turn on discoverable
	sprintf(command, "echo -e 'discoverable on\nquit' | bluetoothctl");
	system(command);

	struct sockaddr_l2 loc_addr = { 0 };
	struct sockaddr_l2 rem_addr = { 0 };
	char buf[1024] = { 0 };
	socklen_t opt = sizeof(rem_addr);

	//allocate socket
	serv_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

	// bind socket to port 0x1001 of the first available 
	// bluetooth adapter
	loc_addr.l2_family = AF_BLUETOOTH;
	loc_addr.l2_bdaddr = *BDADDR_ANY;
	loc_addr.l2_psm = htobs(0x1001);

	bind(serv_sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(serv_sock, 1);

	// accept one connection
	client = accept(serv_sock, (struct sockaddr *)&rem_addr, &opt);
	client_connected = 1;	
	pthread_create(&rcv_handler_theread,
		       NULL,
		       recv_handlerL,
		       NULL);
	return NULL;
}

int sendL (char *msg)
{
	if(!client_connected)
	{
		return -1;
	}
	return write(client, msg, strlen(msg));

}

void *recv_handlerL ()
{
	int checkVal = -1;
	while(1)
	{
		checkVal = read(client, recv_buf ,sizeof(recv_buf));
		if(checkVal < 0)
			break;
		onData(recv_buf); 
	}
	return NULL;	
}

void set_callbackL (callbackStr callback)
{
	onData = callback;
}

