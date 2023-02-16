#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define WATER_SIGNAL A1
#define WATER_POWER 7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(WATER_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(WATER_POWER, LOW);
}

int readWaterSensor() {
  digitalWrite(WATER_POWER, HIGH);
  delay(10);
  int val = analogRead(WATER_SIGNAL);
  digitalWrite(WATER_POWER, LOW);
  return val;
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = readWaterSensor();

  if (val < 450) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
