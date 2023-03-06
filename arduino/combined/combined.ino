#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <utility/wifi_drv.h>
#include <L298N.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
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

Encoder encoder(9, 10);

// Servo pin
#define SERVO 11

// Water level sensor
#define WATER_SIGNAL A1
#define WATER_POWER 12

// joystick pins
#define BUTTON 13

// Limit switches
#define CARAFE 0
#define TOP 1

L298N motor(ENA, IN1, IN2);
long press_pos = -999;

L298N pump(ENB, IN3, IN4);

// Length of time in milliseconds to pump water for
const unsigned long waterInterval = 1000L;

unsigned long lastConnectionTime = 0;

const unsigned long postingInterval = 5L * 1000L;

char ssid[] = "deadass";
char pass[] = "ilovemywife";

char brew_host[] = "fydp.eba-twsqhru6.us-east-1.elasticbeanstalk.com";
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
  motor.setSpeed(100);

  // Pump max flow
  pump.setSpeed(255);

  // Water
  pinMode(WATER_POWER, OUTPUT);
  digitalWrite(WATER_POWER, LOW);

  pinMode(BUTTON, INPUT);
  pinMode(CARAFE, INPUT);
  pinMode(TOP, INPUT);
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
    Serial.println(response);

    String json_str = skipResponseHeaders(response);

    // JSON doc used to deserialize JSON strings
    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, json_str);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    } else {
      Serial.println("Deserialized (json is valid)");

      String action = doc["action"];

      doAction(action);
    }
  }

  int val = readWaterSensor();

  if (val < 450) {
    WiFiDrv::digitalWrite(27, HIGH);
  } else {
    WiFiDrv::digitalWrite(27, LOW);
  }
}

void connectBrewDaddy() {
  client.stop();
  Serial.println("Connecting to brew daddy");
  if (client.connect(brew_host, port)) {
    Serial.print("connected to brew daddy: ");
    Serial.println(brew_host);

    client.println("GET /demo HTTP/1.1");
    client.println("Host: fydp.eba-twsqhru6.us-east-1.elasticbeanstalk.com");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");

    client.println();
    lastConnectionTime = millis();
  } else {
    Serial.println("Connection failed");
  }
}

void doAction(String action) {
  Serial.print("Doing action: ");
  Serial.println(action);

  if (action == "stop" && !brew_stopped) {
    for (int i = 0; i < 5; i++) {
      WiFiDrv::digitalWrite(26, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(26, LOW);
      delay(500);
    }

    //    WiFiDrv::digitalWrite(26, HIGH);
    //
    //    while(digitalRead(BUTTON) != HIGH) {}
    //
    //    controlMotor(-2);
    //
    //    while(digitalRead(BUTTON) != LOW) {}
    //
    //    while(digitalRead(BUTTON) != HIGH) {}
    //
    //    controlMotor(0);
    //
    //    while(digitalRead(BUTTON) != LOW) {}
    //
    //    while(digitalRead(BUTTON) != HIGH) {}
    //
    //    controlMotor(2);
    //
    //    while(digitalRead(BUTTON) != LOW) {}
    //
    //    while(digitalRead(BUTTON) != HIGH) {}
    //
    //    controlMotor(0);
    //
    //    WiFiDrv::digitalWrite(26, LOW);
  } else if (action == "go" && !brew_started) {
    for (int i = 0; i < 5; i++) {
      WiFiDrv::digitalWrite(25, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(25, LOW);
      delay(500);
    }

    startBrew();
  }
}

void startBrew() {
  unsigned long start = millis();
  unsigned long end = start;

  pump.forward();
  // Pump water for a bit
  while ((end - start) < waterInterval) { end = millis(); }
  pump.stop();

  // Dispense grounds
  // Open
  for (hopper_pos = 0; hopper_pos <= 180; hopper_pos += 1) {
    hopper.write(hopper_pos);
    delay(15);
  }
  // Wait for gravity to do work
  delay(500);
  // Close
  for (hopper_pos = 180; hopper_pos >= 0; hopper_pos -= 1) {
    hopper.write(hopper_pos);
    delay(15);
  }

  brew_started = true;
  brew_stopped = false;
}

void stopBrew() {
  motor.forward();

  // Move down until we bottom out
  while (press_pos <= 96000) {
    long new_pos = encoder.read();
    press_pos = new_pos
  }
  
  // Stop motor when we bottom out
  motor.stop();

  // Wait until carafe is removed
  int carafeIn = 1;
  while (carafeState) {carafeState = digitalRead(CARAFE);}

  // Move press to top
  int topHit = 0;
  motor.backward();
  while (!topHit) {topHit = digitalRead(TOP);}

  // Stop motor at top and reset encoder values
  motor.stop();
  encoder.write(0);
  press_pos = -999;
  
  brew_started = false;
  brew_stopped = true;
}

int readWaterSensor() {
  digitalWrite(WATER_POWER, HIGH);
  delay(10);
  int val = analogRead(WATER_SIGNAL);
  digitalWrite(WATER_POWER, LOW);
  return val;
}

void controlMotor(int x) {
  int direction = motor.getDirection();

  if (x == 2) {
    motor.forward();
  } else if (x == -2) {
    Serial.println("setting backwards");
    motor.backward();
  } else {
    motor.stop();
  }
}

String skipResponseHeaders(String response) {
  int i = response.indexOf("\r\n\r\n");
  if (i != -1) {
    String json_str = response.substring(i + 4);
    Serial.print("Skipping headers, we get: ");
    Serial.println(json_str);
    return json_str;
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
