#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
    struct sockaddr_rc addr = { 0 };
    int s, status;
    char message[1024];
    char dest[18] = "34:02:86:60:F1:ED";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    for (;;) { // runs forever
      
      printf("Enter letter to send: ");
      scanf("%s", message); // gets user input
      
      status = write(s, message, strlen(message));
            
      printf("Sent Message\n");
      
    } // end forever loop
    

    close(s);
    return 0;
}
