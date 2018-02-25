#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include <linux/videodev2.h>

#define PORT 0x1001
#define BT_MAC "02:00:1B:D3:2E:51"

// TODO - img_buf / buf naming
static uint8_t *img_buf;

static int xioctl(int fd, int request, void *arg)
{
  int r;

  do r = ioctl (fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

int print_caps(int fd)
{
  struct v4l2_capability caps = {};
  if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps))
    {
      perror("Querying Capabilities");
      return 1;
    }

  printf( "Driver Caps:\n"
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


  /*  struct v4l2_cropcap cropcap = {0};
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
    {
      perror("Querying Cropping Capabilities");
      return 1;
    }

  printf( "Camera Cropping:\n"
          "  Bounds: %dx%d+%d+%d\n"
          "  Default: %dx%d+%d+%d\n"
          "  Aspect: %d/%d\n",
          cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
          cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
          cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);*/

  int support_grbg10 = 0;

  struct v4l2_fmtdesc fmtdesc = {0};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  char fourcc[5] = {0};
  char c, e;
  printf("  FMT : CE Desc\n--------------------\n");
  while (0 == xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
      strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
      if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
        support_grbg10 = 1;
      c = fmtdesc.flags & 1? 'C' : ' ';
      e = fmtdesc.flags & 2? 'E' : ' ';
      printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
      fmtdesc.index++;
    }
  /*
    if (!support_grbg10)
    {
    printf("Doesn't support GRBG10.\n");
    return 1;
    }*/

  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 640;
  fmt.fmt.pix.height = 480;
  //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
  //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
    {
      perror("Setting Pixel Format");
      return 1;
    }

  strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
  printf( "Selected Camera Mode:\n"
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

int init_mmap(int fd)
{
  struct v4l2_requestbuffers req = {0};
  req.count = 1;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
      perror("Requesting Buffer");
      return 1;
    }

  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;
  if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
    {
      perror("Querying Buffer");
      return 1;
    }

  img_buf = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
  printf("Length: %d\nAddress: %p\n", buf.length, img_buf);
  printf("Image Length: %d\n", buf.bytesused);

  return 0;
}

int capture_image(int fd, int my_socket)
{
  int status;
  int i;
  int buf_size = 550;
  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;
  if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    {
      perror("Query Buffer");
      return 1;
    }

  if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
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

  if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
      perror("Retrieving Frame");
      return 1;
    }

 
  printf("buf lenght: %d\n", buf.length);
  for (i = 0; i < buf.length; i++) {
    status = send(my_socket, img_buf + i, buf_size, 0);
    i += buf_size;
    //    printf("byte sent: %d\n", status);
    //usleep(1000);
  }
  
  if( status < 0 ) perror("No data sent");  
  printf ("sent image\n");

  /*   IplImage* frame;
       CvMat cvmat = cvMat(480, 640, CV_8UC3, (void*)img_buf);
       frame = cvDecodeImage(&cvmat, 1);
       cvNamedWindow("window",CV_WINDOW_AUTOSIZE);
       cvShowImage("window", frame);
       cvWaitKey(0);
       cvSaveImage("image.jpg", frame, 0);
  */
  return 0;
}

int main()
{
  struct sockaddr_l2 addr = { 0 };
  int my_socket, status;
  //  char dest[18] = 
  int fd;

  fd = open("/dev/video0", O_RDWR);
  if (fd == -1)
    {
      perror("Opening video device");
      return 1;
    }
  if(print_caps(fd))
    return 1;

  if(init_mmap(fd))
    return 1;

  // allocate a socket
  my_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

  addr.l2_family = AF_BLUETOOTH;
  addr.l2_psm = htobs(PORT);
  str2ba( BT_MAC, &addr.l2_bdaddr );

  // connect to server
  status = connect(my_socket, (struct sockaddr *)&addr, sizeof(addr));
  if( status < 0 ) perror("connection failed");  

  int i;
  for(i=0; i<1; i++)
    {
      if(capture_image(fd, my_socket))
        return 1;
    }
  close(fd);
  return 0;
}
