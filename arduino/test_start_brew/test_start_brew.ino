#include <L298N.h>
#include <Servo.h>
#include <Encoder.h>

// Servo pin
#define SERVO 11

// motor pins
#define ENA 3
#define IN1 4
#define IN2 5

// pump pins
#define ENB 6
#define IN3 7
#define IN4 8

Encoder encoder(9, 10);

L298N motor(ENA, IN1, IN2);
L298N pump(ENB, IN3, IN4);

// Limit switches
#define CARAFE 0
#define TOP 1

long press_pos = -999;

// Length of time in milliseconds to pump water for
const unsigned long waterInterval = 20000L;

// Servo
Servo hopper;
int hopper_pos = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // put your setup code here, to run once:
  hopper.attach(SERVO);

  motor.setSpeed(122);
  pump.setSpeed(255);

  hopper.write(90);
  delay(500);
  Serial.println("Setup done");
}

void loop() {
  Serial.println("Starting brew");
  // put your main code here, to run repeatedly:
  motor.forward();

  // Move down until we bottom out
  while (press_pos <= 95000) {
    long new_pos = encoder.read();
    press_pos = new_pos;
  }

  motor.stop();

  delay(5000);

  motor.backward();

  while (press_pos >= 1000) {
    long new_pos = encoder.read();
    press_pos = new_pos;
  }

  motor.stop();

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

  pump.forward();

  // Dispense grounds
  open_hopper(2, start);

  while ((end - start) < waterInterval) { end = millis(); }
  pump.stop();
}

void open_hopper(int x, int start) {
  for (int i = 0; i < x; i++) {
    Serial.println("OPENING TOP");
    hopper.write(0);
    delay(4000);
      
    // Wait for gravity to do work
    hopper.write(90);
    delay(4000);
  
    Serial.println("OPENING BOTTOM");
  
    hopper.write(180);
    delay(4000);
  
    hopper.write(90);
    delay(1000);

    unsigned long end = millis();
    if ((end - start) >= waterInterval) {
      pump.stop();
    }
  }
}

void stopBrew() {
  motor.forward();

  // Move down until we bottom out
  while (press_pos <= 95000) {
    long new_pos = encoder.read();
    press_pos = new_pos;
  }
  
  // Stop motor when we bottom out
  motor.stop();

  // Wait until carafe is removed
  int carafeState = LOW;
  while (carafeState == LOW) {carafeState = digitalRead(CARAFE);}
  delay(2000);

  // Move press to top
  int topHit = HIGH;
  motor.backward();
  while (topHit == HIGH) {topHit = digitalRead(TOP);}

  // Stop motor at top and reset encoder values
  motor.stop();
  encoder.write(0);
  press_pos = -999;
}
