/*
  Connect Arduino WiFi to AskSensors
 * Description:  This sketch connects the Arduino to AskSensors IoT Platform (https://asksensors.com) using an ESP8266 WiFi.
 *  Author: https://asksensors.com, 2018
 *  github: https://github.com/asksensors
 */
 

#include <SoftwareSerial.h>

// serial config
#define     RX    10
#define     TX    11
SoftwareSerial AT(RX,TX);

// TODO: change user config
String ssid     = "52"; //Wifi SSID
String password = "D8AABRogers"; //Wifi Password
const unsigned int writeInterval = 25000; // write interval (in ms)

// ASKSENSORS API host config
String host = "wifitest.adafruit.com";  // API host name
String port = "80";      // port

int AT_cmd_time; 
boolean AT_cmd_result = false; 

void setup() {
  Serial.begin(9600);
  // open serial 
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect Arduino WiFi");
  AT.begin(9600);
  delay(1000);
  Serial.println("Initiate Lua commands with ESP8266 ");

  Serial.println("Test connection");
  AT.println("print(\"Hi I'm here\")");
  delay(2000);
  
  Serial.println(AT.readStringUntil("\n"));
  

  // Set to wifi mode
  Serial.println("Set wifi mode");
  AT.println("wifi.setmode(wifi.STATION)");

  delay(5000);

  Serial.println("Set wifi credentials");
  AT.println("wifi.sta.config(\"52\",\"D8AABRogers\"");

  delay(5000);

  Serial.println("Connect to wifi");
  AT.println("wifi.sta.connect()");

  delay(10000);

  Serial.println("Get status");
  AT.println("print(wifi.sta.status())");
  
  Serial.println(AT.read());
  if(AT.find("5")) {
    Serial.println("Connection successful");
  }

  Serial.println("Fuck");
}

void loop() {
}



// sendATcmd
void sendATcmd(String AT_cmd, int AT_cmd_maxTime, char readReplay[]) {
  Serial.print("AT command:");
  Serial.println(AT_cmd);

  while(AT_cmd_time < (AT_cmd_maxTime)) {
    AT.println(AT_cmd);
    if(AT.find(readReplay)) {
      AT_cmd_result = true;
      break;
    }
  
    AT_cmd_time++;
  }
  Serial.print("...Result:");
  if(AT_cmd_result == true) {
    Serial.println("DONE");
    AT_cmd_time = 0;
  }
  
  if(AT_cmd_result == false) {
    Serial.println("FAILED");
    AT_cmd_time = 0;
  }
  
  AT_cmd_result = false;
 }
