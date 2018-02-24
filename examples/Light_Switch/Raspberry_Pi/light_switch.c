#include <stdio.h>
#include <wiringPi.h>

#include <ads1115.h>
#define MY_BASE 2222

#define BUTTON 4
#define LED 17

int main(int argc, char* argv[]) {

  wiringPiSetupGpio();

  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  pullUpDnControl(BUTTON, PUD_UP);
  ads1115Setup (MY_BASE, 0x48);
  
  // Main loop
  while(1) {
    if (digitalRead(BUTTON)) {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }

    int ch0 = analogRead (MY_BASE + 0) ;
    printf("X: %d\n", ch0);

    usleep(100000);
  }
}
