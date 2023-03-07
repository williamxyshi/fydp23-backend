#include <L298N.h>
#include <Servo.h>

// Servo pin
#define SERVO 11

// pump pins
#define ENB 6
#define IN3 7
#define IN4 8

L298N pump(ENB, IN3, IN4);

// Length of time in milliseconds to pump water for
const unsigned long waterInterval = 20000L;

// Servo
Servo hopper;
int hopper_pos = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // put your setup code here, to run once:
  hopper.attach(SERVO, 500, 2500);

  pump.setSpeed(255);
  Serial.println("Setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
  startBrew();

  delay(5000);
}

void sweepServo(Servo &servo, int start, int finish) {
  if (start < finish) {
    for (servo_pos = start; servo_pos <= finish; servo_pos += 1) {
      servo.write(servo_pos);
      delay(15);
    }
  }
  if (start > finish) {
    for (servo_pos = start; servo_pos >= finish; servo_pos -= 1) {
      servo.write(servo_pos);
      delay(15);
    }
  }
}

void startBrew() {
  Serial.println("Starting brew");
  unsigned long start = millis();
  unsigned long end = start;

//  pump.forward();

  // Dispense grounds
  // Open first
  for (hopper_pos = 0; hopper_pos <= 180; hopper_pos += 1) {
    hopper.write(hopper_pos);
    Serial.println(hopper.read());
    delay(15);

    end = millis();
    if ((end-start) >= waterInterval) {
      pump.stop();
    }
    
  }  
  // Wait for gravity to do work
  delay(500);
  
  // Close
  for (hopper_pos = 180; hopper_pos >= 0; hopper_pos -= 1) {
    hopper.write(hopper_pos);
    Serial.println(hopper.read());
    delay(15);

    end = millis();
    if ((end-start) >= waterInterval) {
      pump.stop();
    }
  }
//
//  while ((end - start) < waterInterval) { end = millis(); }
//  pump.stop();
}
