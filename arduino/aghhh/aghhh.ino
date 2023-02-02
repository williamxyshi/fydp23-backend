#include <SPI.h>
#include <Ethernet.h>
#include <Robojax_L298N_DC_motor.h>
#include <Servo.h>
#include "HX711.h"

// MOTOR STUFF STARTS ------------------------------
// motor 1 settings
#define IN1 2
#define IN2 3
#define ENA 12 // this pin must be PWM enabled pin

// motor 2 settings
#define IN3 4
#define IN4 5
#define ENB 6 // this pin must be PWM enabled pin

// Servo motor for hopper
Servo servo;

const int CCW = 2; // do not change
const int CW  = 1; // do not change
#define motor1 1 // do not change
#define motor2 2 // do not change

Robojax_L298N_DC_motor motor(IN1, IN2, ENA, IN3, IN4, ENB, true);

/*
 * The left motor and the right motor will be turning in opposite directions when the robot is moving forward.
 * This function will encapsulate the motor control so that direction does not need to be constantly defined. (e.g. CW? or CCW?)
 * Arguments are -100 to 100 (negative values indicate reversing)
 */
void changeMotorPower(int left, int right) {
  
  if (left != 0) {
    motor.rotate(motor1, left > 100 ? 100 : abs(left), left > 0 ? CCW : CW);
  } else {
    motor.brake(motor1);
  }
  
  if (right != 0) {
    motor.rotate(motor2, right > 100 ? 100 : abs(right), right > 0 ? CCW : CW);
  } else {
    motor.brake(motor2);
  }
}
// MOTOR STUFF ENDS --------------------------------

// LOAD SENSOR STUFF BEGINS ------------------------
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

// LOAD SENSOR STUFF ENDS --------------------------



// WATER SENSOR BEGINS -----------------------------
#define POWER_PIN  7
#define SIGNAL_PIN A5

int wetness = 0; // variable to store the sensor value


// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger";
String queryString = "?value1=26&value2=70";;

void setup() {
  Serial.begin(9600);

  // initialize the Ethernet shield using DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address using DHCP");
    while(true);
  }

  // connect to web server on port 80:
  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println(HTTP_METHOD + " " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }

  // Press and pump motor setup
  motor.begin();

  // Hopper servo setup
  servo.attach(8);
  servo.write(0);

  // Load sensor setup
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Water sensor setup
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
}

void loop() {

}
