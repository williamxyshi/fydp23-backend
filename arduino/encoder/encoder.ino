#include <L298N.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>


// motor pins
#define ENA 3
#define IN1 4
#define IN2 5

// joystick pins
#define JOY_X A0

L298N motor(ENA, IN1, IN2);
 
 // Rotary Encoder Inputs
 #define inputCLK 8
 #define inputDT 9

 #define GREEN 25
 #define RED 26
 #define BLUE 27

 int counter = 0; 
 int currentStateCLK;
 int previousStateCLK; 

 String encdir ="";

 void setup() { 
   
   // Set encoder pins as inputs  
   pinMode (inputCLK,INPUT);
   pinMode (inputDT,INPUT);
   
   // RGB LED'S
   WiFiDrv::pinMode(GREEN, OUTPUT);
   WiFiDrv::pinMode(RED, OUTPUT);  
   WiFiDrv::pinMode(BLUE, OUTPUT);  
   
   // Setup Serial Monitor
   Serial.begin (9600);
   
   // Read the initial state of inputCLK
   // Assign to previousStateCLK variable
   previousStateCLK = digitalRead(inputCLK);

   motor.setSpeed(100);
   
   Serial.println("Setup done");
 }

 void controlMotor(int x) {
  int direction = motor.getDirection();

  if (x == 2) {
    motor.forward();
  }
  else if (x == -2) {
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
  
  // Read the current state of inputCLK
   currentStateCLK = digitalRead(inputCLK);
    
   // If the previous and the current state of the inputCLK are different then a pulse has occured
   if (currentStateCLK != previousStateCLK){ 
       
     // If the inputDT state is different than the inputCLK state then 
     // the encoder is rotating counterclockwise
     if (digitalRead(inputDT) != currentStateCLK) { 
       counter --;
       encdir ="CCW";
       WiFiDrv::digitalWrite(RED, LOW);
       WiFiDrv::digitalWrite(GREEN, HIGH);
       
     } else {
       // Encoder is rotating clockwise
       counter ++;
       encdir ="CW";
       WiFiDrv::digitalWrite(RED, HIGH);
       WiFiDrv::digitalWrite(GREEN, LOW);
       
     }
     Serial.print("Direction: ");
     Serial.print(encdir);
     Serial.print(" -- Value: ");
     Serial.println(counter);
   } 
   // Update previousStateCLK with the current state
   previousStateCLK = currentStateCLK; 
 }
