#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <utility/wifi_drv.h>
#include <L298N.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Encoder.h>

// motor pins
#define ENA 3
#define IN1 4
#define IN2 5

// pump pins
#define ENB 6
#define IN3 7
#define IN4 8

// Yellow 9, Green 10
Encoder encoder(9, 10);

// Servo pin
#define SERVO 11

// Water level sensor
#define WATER_SIGNAL A1
#define WATER_POWER 12

// Limit switches
#define CARAFE 13
#define TOP 2

L298N motor(ENA, IN1, IN2);
long press_pos = -999;

L298N pump(ENB, IN3, IN4);

// Length of time in milliseconds to pump water for
unsigned long waterInterval = 22500L;

// Number of times to open and close the hopper
int num_dispenses = 5;

// Action can be 'go', 'stop' or 'wait'
String action = "wait";

unsigned long lastConnectionTime = 0;

const unsigned long postingInterval = 5L * 1000L;

char ssid[] = "uuueeeeebbBBGgghh";//"deadass";
char pass[] = "Conradgrebel69"; //"ilovemywife";

char brew_host[] =  "192.168.0.120";//"fydp.eba-twsqhru6.us-east-1.elasticbeanstalk.com";
int port = 80;  // port

int status = WL_IDLE_STATUS;  //connection status
WiFiServer server(80);        //server socket

WiFiClient client = server.available();

// Servo
Servo hopper;

int hopper_pos = 0;

// Flag to detect if brew is already started
bool brew_started = false;
bool brew_stopped = true;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  hopper.attach(SERVO);

  // RGB LED's
  WiFiDrv::pinMode(25, OUTPUT);  //GREEN
  WiFiDrv::pinMode(26, OUTPUT);  //RED
  WiFiDrv::pinMode(27, OUTPUT);  //BLUE

  enable_WiFi();
  connect_WiFi();

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // Motor
  motor.setSpeed(122);

  // Pump max flow
  pump.setSpeed(255);

  // Water
  pinMode(WATER_POWER, OUTPUT);
  digitalWrite(WATER_POWER, LOW);

  pinMode(CARAFE, INPUT_PULLUP);
  pinMode(TOP, INPUT_PULLUP);

  int topHit = LOW;
  motor.backward();
  
  while (topHit == LOW) { topHit = digitalRead(TOP); }

  motor.stop();
  encoder.write(0);
}

void loop() {
  if (millis() - lastConnectionTime > postingInterval) {
    connectBrewDaddy();
  }
  String response = "";
  while (client.available()) {
    response += char(client.read());
  }

  if (response != "") {
    String json_str = response;

    if (response.startsWith("HTTP")) {
      json_str = skipResponseHeaders(response);
    }
    
    // JSON doc used to deserialize JSON strings
    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, json_str);

    if (!error) {
      Serial.println("Deserialized (json is valid)");

      String response_action = doc["action"];
      action = response_action;
      
      num_dispenses = doc["num_dispenses"];
      Serial.println(num_dispenses);
      
      waterInterval = doc["pump_time"];
      Serial.println(waterInterval);

      doAction();
    }
  }

//  int val = readWaterSensor();
//
//  if (val < 450) {
//    WiFiDrv::digitalWrite(27, HIGH);
//  } else {
//    WiFiDrv::digitalWrite(27, LOW);
//  }
}

void connectBrewDaddy() {
  client.stop();
  Serial.println("Connecting to brew daddy");
  if (client.connect(brew_host, port)) {
    Serial.print("connected to brew daddy: ");
    Serial.println(brew_host);

    client.println("GET /action HTTP/1.1");
    client.println("Host: 192.168.0.120");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");

    client.println();
    lastConnectionTime = millis();
  } else {
    Serial.println("Connection failed");
  }
}


void pingStart() {
  client.stop();
  Serial.println("Pinging endpoint to start brew");
  if (client.connect(brew_host, port)) {
    Serial.print("connected to brew daddy: ");
    Serial.println(brew_host);

    client.println("GET /start HTTP/1.1");
    client.println("Host: 192.168.0.120");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");

    client.println();
    lastConnectionTime = millis();

    String response = "";
    // Drain response
    while (client.available()) {
      response += char(client.read());
    }
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}

void pingStop() {
  client.stop();
  Serial.println("Pinging endpoint to finish brew");
  if (client.connect(brew_host, port)) {
    Serial.print("connected to brew daddy: ");
    Serial.println(brew_host);

    client.println("GET /finish HTTP/1.1");
    client.println("Host: 192.168.0.120");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");

    client.println();
    lastConnectionTime = millis();

    String response = "";
    // Drain response
    while (client.available()) {
      response += char(client.read());
    }
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}

void doAction() {
  Serial.print("Doing action: ");
  Serial.println(action);

  if (action == "stop" && !brew_stopped) {
    for (int i = 0; i < 5; i++) {
      WiFiDrv::digitalWrite(26, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(26, LOW);
      delay(500);
    }
    stopBrew();
  } else if (action == "go" && !brew_started) {
    for (int i = 0; i < 5; i++) {
      WiFiDrv::digitalWrite(25, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(25, LOW);
      delay(500);
    }

    startBrew();

    Serial.println("Brew started");
    pingStart();
  }
}

void startBrew() {
  Serial.println("Starting brew");
  unsigned long start = millis();
  unsigned long end = start;

  pump.forward();

  // Dispense grounds
  open_hopper(start);

  while ((end - start) < waterInterval) { end = millis(); }
  pump.stop();

  brew_started = true;
  brew_stopped = false;
}

void stopBrew() {
  motor.forward();

  // Move down until we bottom out
  while (press_pos <= 105000) {
    press_pos = encoder.read();
  }
  
  // Stop motor when we bottom out
  motor.stop();
  pingStop();

  // Wait until carafe is removed
  int carafeState = HIGH;
  while (carafeState == HIGH) { 
    carafeState = digitalRead(CARAFE);
    if(carafeState == LOW) {
      for(int i = 0; i < 5; i ++ ) {
        carafeState = digitalRead(CARAFE);
        if(carafeState == HIGH) {
          break;
        }
      }
    }
  }
  delay(3000);

  // Move press to top
  int top = LOW;
  motor.backward();
  
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

  // Stop motor at top and reset encoder values
  motor.stop();
  encoder.write(0);
  press_pos = -999;
  
  brew_started = false;
  brew_stopped = true;
}

// Opens hopper while checking if we should stop the pump
void open_hopper(unsigned long start) {
  for (int i = 0; i < num_dispenses; i++) {
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
      pump.stop();
    }
  }
}

int readWaterSensor() {
  digitalWrite(WATER_POWER, HIGH);
  delay(10);
  int val = analogRead(WATER_SIGNAL);
  digitalWrite(WATER_POWER, LOW);
  return val;
}

String skipResponseHeaders(String response) {
  int i = response.indexOf("\r\n\r\n");
  if (i != -1) {
    String return_str = response.substring(i + 4);
    Serial.print("Skipping headers, we get: ");
    Serial.println(return_str);
    return return_str;
  }
  return "";
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}
