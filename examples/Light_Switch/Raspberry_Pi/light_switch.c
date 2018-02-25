
#include <stdio.h>
#include <wiringPi.h>

#include <ads1115.h>
#define MY_BASE 2222

#define BUTTON 4
#define LED 17
#define POT_INTERVAL 103
#define DEBOUNCE_THRESH 200

static last_isr_time = 0;

void buttonISR(void) {

  unsigned long isr_time = millis();

  if (isr_time - last_isr_time > DEBOUNCE_THRESH) {
    usleep(100000); // ISR happens too fast when going 0 to 1
    if (digitalRead(BUTTON)) {
      puts("button on");
    } else {
      puts("button off");
    }
  }

  last_isr_time = isr_time;
}

int main(int argc, char* argv[]) {

  int pot_in;
  wiringPiSetupGpio();

  // Setup button
  pinMode(BUTTON, INPUT);
  pullUpDnControl(BUTTON, PUD_UP);
  wiringPiISR (BUTTON, INT_EDGE_BOTH, &buttonISR);

  // Setup LED light
  pinMode(LED, OUTPUT);

  // Setup Analog input from ads1115
  ads1115Setup (MY_BASE, 0x48);

  // Main loop
  while(1) {

    pot_in = analogRead(MY_BASE + 0)/POT_INTERVAL;
    printf("X: %d\n", pot_in);

    usleep(100000);
  }
}
