#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include "unitooth.h"

int master(int argc, char **argv)
{
	struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 };
	char buf[1024] = { 0 };
	int s, client, bytes_read;
	socklen_t opt = sizeof(rem_addr);

	//allocate socket
		s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

	// bind socket to port 0x1001 of the first available 
	// bluetooth adapter
	loc_addr.l2_family = AF_BLUETOOTH;
	loc_addr.l2_bdaddr = *BDADDR_ANY;
	loc_addr.l2_psm = htobs(0x1001);

	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);
	
	

	ba2str( &rem_addr.l2_bdaddr, buf );
	fprintf(stderr, "accepted connection from %s\n", buf);

	memset(buf, 0, sizeof(buf));

	// read data from the client
	bytes_read = read(client, buf, sizeof(buf));
	if( bytes_read > 0 ) {
		printf("received [%s]\n", buf);
	}


	// close connection
	close(client);
	close(s);
}

int slave(int argc, char **argv)
{
	struct sockaddr_l2 addr = { 0 };
	int s, status;
	//char *message = "hello!";
	char dest[18] = "01:23:45:67:89:AB";

	if(argc < 2)
	{
		fprintf(stderr, "usage: %s <bt_addr>\n", argv[0]);
		exit(2);
	}

	strncpy(dest, argv[1], 18);

	//allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

	// set the connection parameters (who to connect to)
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(0x1001);
	str2ba( dest, &addr.l2_bdaddr );

	// connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	if( status == 0 ) {
		status = write(s, "hello!", 6);
	}

	if( status < 0 ) perror("uh oh");

	close(s);
}

