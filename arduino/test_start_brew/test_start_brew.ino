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
#define CARAFE 13
#define TOP 2

long press_pos = -999;

// Length of time in milliseconds to pump water for
unsigned long waterInterval = 10000L;

// Number of times to open and close the hopper
int num_dispenses = 3;

// Servo
Servo hopper;
int hopper_pos = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Here");
  
  // put your setup code here, to run once:
  hopper.attach(SERVO);

  motor.setSpeed(122);
  pump.setSpeed(255);

  hopper.write(90);
  delay(500);
  Serial.println("Setup done");

  pinMode(CARAFE, INPUT_PULLUP);
  pinMode(TOP, INPUT_PULLUP);
}

void loop() {
  Serial.println("Starting brew");

  // Wait until top is hit
  int top = LOW;
  motor.backward();

  // Debounce
  while (top == LOW) {
    top = digitalRead(TOP);
    if(top == HIGH) {
      for(int i = 0; i < 5; i ++ ) {
        top = digitalRead(TOP);
        if(top == LOW) {
          break;
        }
      }
    }
  }

  motor.stop();

  Serial.println("top has been hit :)");

  encoder.write(0);

  int carafeState = LOW;
  while (carafeState == LOW) { 
    carafeState = digitalRead(CARAFE);
    Serial.println("Carafe is low");
    if(carafeState == HIGH) {
      for(int i = 0; i < 5; i ++ ) {
        carafeState = digitalRead(CARAFE);
        if(carafeState == LOW) {
          break;
        }
      }
    }
  }

  unsigned long start = millis();
  unsigned long end = start;

  pump.forward();
  open_hopper(start);

  while ((end - start) < waterInterval) {
    end = millis(); 
    Serial.print("Running pump: ");
    Serial.println(end - start);
  }
  pump.stop();

  delay(5000);

  motor.forward();

  int press_rate = 0;
  
  Serial.println("Motor should be running");
  // Move down until we bottom out
  while (press_pos <= 105000) {
    long new_pos = encoder.read();
//    
//    for (int i = 0; i < 10; i++) {
//      delay(50);
//      new_pos = encoder.read();
//    }
//    if (new_pos > 95000 && new_pos == press_pos) {
//      break;
//    }
//    
    press_pos = new_pos;
  }
  // Stop motor when we bottom out
  motor.stop();


  Serial.println("Waiting for carafe to be removed");
  carafeState = HIGH;
  while (carafeState == HIGH) { carafeState = digitalRead(CARAFE); }
  delay(2000);

  // Wait until top is hit
  top = LOW;
  
  motor.backward();
  
  while (top == LOW) {
    top = digitalRead(TOP);
    if(top == HIGH) {
      for(int i = 0; i < 5; i ++ ) {
        top = digitalRead(TOP);
        Serial.print("deb");
        Serial.println(top);
        if(top == LOW) {
          break;
        }
      }
    }
    Serial.println(top);
  }

  motor.stop();
  press_pos = -999;
  encoder.write(0);

  while(true) {}
}

// Opens hopper while checking if we should stop the pump
void open_hopper(unsigned long start) {
  for (int i = 0; i < 3; i++) {
    Serial.println("OPENING TOP");
    hopper.write(0);
    waitForDurationAndCheckToStopPump(2000, start);
    //delay(2000);

    Serial.println("OPENING BOTTOM");
    hopper.write(180);
    waitForDurationAndCheckToStopPump(2000, start);
  } 

  Serial.println("Returning to neutral");
  hopper.write(90);
  waitForDurationAndCheckToStopPump(2000, start);
}

void waitForDurationAndCheckToStopPump(int duration, unsigned long start) {
  unsigned long x = millis();
  unsigned long end = x;
  while ((end - x) < duration) {
    end = millis();
    if ((end - start) >= waterInterval) {
      Serial.print("Stopping after ");
      Serial.print(end - start);
      Serial.println("milliseconds.");
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
