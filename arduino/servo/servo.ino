#include <Servo.h>

Servo hopper;

int hopper_pos = 0;

void setup() {
  // put your setup code here, to run once:
  hopper.attach(9); // Pin 9 
}

void loop() {
  // put your main code here, to run repeatedly:
  for (hopper_pos = 0; hopper_pos <= 180; hopper_pos += 1) {
    hopper.write(hopper_pos);
    delay(15);
  }
}
