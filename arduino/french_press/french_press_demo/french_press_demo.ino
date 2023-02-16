#include <L298N.h>

// motor pins
#define ENA 3
#define IN1 4
#define IN2 5

// joystick pins
#define JOY_X A0

L298N motor(ENA, IN1, IN2);

void setup() {
  // put your setup code here, to run once:
  float motorSpeed = floor(1/2 * 255);
  motor.setSpeed(100);

  Serial.begin(9600);
}

void controlMotor(int x) {
  int direction = motor.getDirection();

  if (x == 2) {
    motor.forward();
  }
  else if (x == -2) {
    Serial.println("setting backwards");
    motor.backward();
  }
  else {
    motor.stop();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float joystick_v = analogRead(JOY_X);
  int x = map(joystick_v, 0, 1023, -2, 2); // map the joystick voltage to (-2,2))
  
  controlMotor(x);
}