#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>  // socket, setsockopt, accept, send, recv
#include <sys/socket.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <signal.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "unitooth.h"

#include <linux/videodev2.h>

pthread_t server_thread;
pthread_t rcv_handler_theread;

static int serv_sock; 		// server socket
static int client_connected = 0;// boolean to signify that client is connected
static int client = -1;		// client fd
static callbackStr onData = NULL;
static char recv_buf[1024];
static char buffer_plus[602];
static int fd; // camera fd

static void *server_daemonR();
static void *recv_handlerR();
static int init_mmap();
static int print_caps();
static int xioctl(int request, void *arg);

static uint8_t *img_buf;
static char img_meta[32];

struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

int serverR()
{
  return pthread_create(&server_thread,
			NULL,
			server_daemonR,
		       	NULL);
}

void *server_daemonR()
{
  int status;
  char command[64];

  // Turn on discoverable
  sprintf(command, "echo -e 'discoverable on\nquit' | bluetoothctl");
  system(command);

  //char buf[1024] = { 0 };
  socklen_t opt = sizeof(rem_addr);

  //allocate socket
  serv_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  fprintf(stdout, "socket %d\n", serv_sock);  
  
  // bind socket to port 1 of the first available 
  // bluetooth adapter
  loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  loc_addr.rc_channel = (uint8_t) 1;

  status = bind(serv_sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
  fprintf(stdout, "bind %d\n", status);

  // put socket into listening mode
  status = listen(serv_sock, 3);
  fprintf(stdout, "Listen  %d\n", status);
  
  // accept one connection
  client = accept(serv_sock, (struct sockaddr *)&rem_addr, &opt);
  client_connected = 1;
  fprintf(stdout, "CONNECTED %d\n",client);
  pthread_create(&rcv_handler_theread,
		 NULL,
		 recv_handlerR,
		 NULL);
  return NULL;
}

int sendR (char *msg)
{
	if(!client_connected)
	{
		return -1;
	}
	return write(client, msg, strlen(msg));

}

void *recv_handlerR()
{
  fprintf(stdout, "server is up %d\n", serv_sock);
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

void set_callbackR (callbackStr callback)
{
	onData = callback;
}


int xioctl(int request, void *arg)
{
  int r;

  do r = ioctl (fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

int print_caps()
{
  struct v4l2_capability caps = {};
  if (-1 == xioctl(VIDIOC_QUERYCAP, &caps))
    {
      perror("Querying Capabilities");
      return 1;
    }

  fprintf(stdout, "Driver Caps:\n"
          "  Driver: \"%s\"\n"
          "  Card: \"%s\"\n"
          "  Bus: \"%s\"\n"
          "  Version: %d.%d\n"
          "  Capabilities: %08x\n",
          caps.driver,
          caps.card,
          caps.bus_info,
          (caps.version>>16)&&0xff,
          (caps.version>>24)&&0xff,
          caps.capabilities);

  int support_grbg10 = 0;

  struct v4l2_fmtdesc fmtdesc = {0};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  char fourcc[5] = {0};
  char c, e;
  printf("  FMT : CE Desc\n--------------------\n");
  while (0 == xioctl(VIDIOC_ENUM_FMT, &fmtdesc))
    {
      strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
      if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
        support_grbg10 = 1;
      c = fmtdesc.flags & 1? 'C' : ' ';
      e = fmtdesc.flags & 2? 'E' : ' ';
      printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
      fmtdesc.index++;
    }

  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 480;
  fmt.fmt.pix.height = 320;
  //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
  //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (-1 == xioctl(VIDIOC_S_FMT, &fmt))
    {
      perror("Setting Pixel Format");
      return 1;
    }

  strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
  fprintf(stdout, "Selected Camera Mode:\n"
          "  Width: %d\n"
          "  Height: %d\n"
          "  PixFmt: %s\n"
          "  Field: %d\n",
          fmt.fmt.pix.width,
          fmt.fmt.pix.height,
          fourcc,
          fmt.fmt.pix.field);
  return 0;
}

int init_mmap()
{
  struct v4l2_requestbuffers req = {0};
  req.count = 1;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(VIDIOC_REQBUFS, &req))
    {
      perror("Requesting Buffer");
      return 1;
    }

  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;
  if(-1 == xioctl(VIDIOC_QUERYBUF, &buf))
    {
      perror("Querying Buffer");
      return 1;
    }

  img_buf = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
  fprintf(stdout, "Length: %d\nAddress: %p\n", buf.length, img_buf);
  
  return 0;
}

int capture_init() {
  fd = open("/dev/video0", O_RDWR);
  if (fd == -1)
    {
      perror("Opening video device");
      return 1;
    }
  if(print_caps())
    return 1;
  
  if(init_mmap())
    return 1;

  fprintf(stdout,"done %d\n", fd);
  
  return 0;
}

int capture_image()
{
  int status;
  int i, j = 0;
  int buf_size = 600;
  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;
  if(-1 == xioctl(VIDIOC_QBUF, &buf))
    {
      perror("Query Buffer");
      return 1;
    }

  if(-1 == xioctl(VIDIOC_STREAMON, &buf.type))
    {
      perror("Start Capture");
      return 1;
    }
  
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  struct timeval tv = {0};
  tv.tv_sec = 2;
  int r = select(fd+1, &fds, NULL, NULL, &tv);
  if(-1 == r)
    {
      perror("Waiting for Frame");
      return 1;
    }

  if(-1 == xioctl(VIDIOC_DQBUF, &buf))
    {
      perror("Retrieving Frame");
      return 1;
    }

  /*
  pFile = fopen("test.jpeg","wb");
  if (pFile) {
    fwrite(img_buf, buf.length, 1, pFile);
    puts("Saved jpg");
  } else {
    puts("Can't open file");
  }  
  */
  
  fprintf(stdout, "--Image Length: %d\n", buf.bytesused);
  
  sprintf(img_meta, "%d",  buf.bytesused);
  memcpy(img_meta + 4, "\0", 1);
  
  write(client, img_meta, strlen(img_meta));
  //  status = sendto(client, img_meta, 4, 0, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
  
  for (i = 0, j=0; i < buf.bytesused; i += buf_size, j++) {
      //  printf("T: %d\n", j);
    //status = sendto(client, img_buf + i, buf_size, 0, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    write(client, img_buf + i, buf_size);
  }
  
  //  if( status < 0 ) perror("No data sent");  
  printf ("sent image: %d bytes\n", i);
  
  return 0;
}

void unitooth_close() {
  close(client);
  close(serv_sock);
}
