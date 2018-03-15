#include <stdio.h>
#include "gpio.h"
#include "../../../src/Linux/unitooth.h"

#define A1 28
#define A2 33
#define B1 35
#define B2 34

int Seq0[4] = {0,1,0,0};
int Seq1[4] = {0,1,0,1};
int Seq2[4] = {0,0,0,1};
int Seq3[4] = {1,0,0,1};
int Seq4[4] = {1,0,0,0};
int Seq5[4] = {1,0,1,0};
int Seq6[4] = {0,0,1,0};
int Seq7[4] = {0,1,1,0};

void onData(char* value) {

  printf("value: %d\n", value);
  
}

void setStep(int a, int b, int c, int d) {
  GpioSetValue(A1, a);
  GpioSetValue(A2, b);
  GpioSetValue(B1, c);
  GpioSetValue(B2, d);
}

int main()
{
  int fd;
  char test[32];

  GpioEnable(A1);
  GpioEnable(A2);
  GpioEnable(B1);
  GpioEnable(B2);
  GpioSetDirection(A1, OUT);
  GpioSetDirection(A2, OUT);
  GpioSetDirection(B1, OUT);
  GpioSetDirection(B2, OUT);

  scanf("%s", test);
  setStep(Seq0[0], Seq0[1], Seq0[2], Seq0[3]);
  scanf("%s", test);
  setStep(Seq1[0], Seq1[1], Seq1[2], Seq1[3]);

  
  serverR();
  set_callbackR(onData);

  capture_init();

  puts("wait");
  scanf("%s", test);
  puts("go!");
  
  //while(1){
  capture_image();
  while(1) { }
  puts("close");
  unitooth_close();
  return 0;
}
