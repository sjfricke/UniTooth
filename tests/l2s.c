#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char **argv)
{
  FILE* pFile;
  char* fileName = "test2.jpeg";
  struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 };
  char* buf;
  int s, client, bytes_read;
  socklen_t opt = sizeof(rem_addr);
  char command[64];
  char* image_buf;
  char image_size_str[4];
  int packet_size = 600;
  int image_size;
  int t = 0;
  int k = 0;


  image_buf = malloc(48000); // risky mofo
  buf = malloc(1024);
  
  // Turn on discoverable
  sprintf(command, "echo -e 'discoverable on\nquit' | bluetoothctl");
  system(command);
    
  // allocate socket
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

  bytes_read = read(client, buf, 4);
  //  printf("TEST: %d\n", buf);
  image_size = atoi(buf) / 10;
  printf("omgd: %d\n", image_size);
  t = 0;
  k = 0;
  while(1) {
    //      memset(buf, 0, sizeof(buf));
    // read data from the client
    bytes_read = read(client, buf, packet_size);
    //    printf("Test: %d - %d - %d\n",(uint8_t)(*buf),  (uint8_t)(*buf) * packet_size, t);
    memcpy(image_buf + (k * packet_size), buf, packet_size);
    
    t += bytes_read;
    k++;
    if( t >= image_size ) {
      printf("received %d bytes\n", t);
      break;
    }
  }

  pFile = fopen(fileName,"wb");
  if (pFile) {
    fwrite(image_buf, image_size, 1, pFile);
    puts("Saved jpg");
  } else {
    puts("Can't open file");
  }
    
  // close connection
  close(client);
  close(s);
}

