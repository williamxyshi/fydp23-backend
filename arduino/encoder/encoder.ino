#include <L298N.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include <Encoder.h>


// motor pins
#define ENA 3
#define IN1 4
#define IN2 5

// joystick pins
#define JOY_X A0

L298N motor(ENA, IN1, IN2);
 
 // Rotary Encoder Inputs
 #define CLK 9
 #define DT 8

 Encoder encoder(9, 8);

 #define GREEN 25
 #define RED 26
 #define BLUE 27

 long pos_motor = -999;

 void setup() { 

   // RGB LED'S
   WiFiDrv::pinMode(GREEN, OUTPUT);
   WiFiDrv::pinMode(RED, OUTPUT);  
   WiFiDrv::pinMode(BLUE, OUTPUT);  
   
   // Setup Serial Monitor
   Serial.begin (9600);

   motor.setSpeed(170);
   
   Serial.println("Setup done");
 }

 void controlMotor(int x) {
  int direction = motor.getDirection();

  if (x > 0) {
    motor.forward();
  }
  else if (x < -1) {
    motor.backward();
  }
  else {
    motor.stop();
  }
}

 void loop() {
  
  float joystick_v = analogRead(JOY_X);
  int x = map(joystick_v, 0, 1023, -2, 2); // map the joystick voltage to (-2,2))

  controlMotor(x);

  long new_pos = encoder.read();

  if (new_pos != pos_motor) {
    Serial.print("Motor position: ");
    Serial.println(new_pos);

    pos_motor = new_pos;
  }
  
//  // Read the current state of inputCLK
//  currentStateCLK = digitalRead(CLK);
//    
//  // If last and current state of CLK are different, then pulse occurred
//  // React to only 1 state change to avoid double count
//  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
//
//    // If the DT state is different than the CLK state then
//    // the encoder is rotating CCW so decrement
//    if (digitalRead(DT) != currentStateCLK) {
//      counter --;
//      currentDir ="CCW";
//    } else {
//      // Encoder is rotating CW so increment
//      counter ++;
//      currentDir ="CW";
//    }
//
//    Serial.print("Direction: ");
//    Serial.print(currentDir);
//    Serial.print(" | Counter: ");
//    Serial.println(counter);
//  }
//
//  // Remember last CLK state
//  lastStateCLK = currentStateCLK;
 }
