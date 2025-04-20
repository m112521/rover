/**
https://github.com/madhephaestus/ESP32Servo/
*/
#include <TB6612_ESP32.h>
#include <ESP32Servo.h>
#include <TB6612_ESP32.h>

#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB

#define SERVO1_PIN 5
#define SERVO2_PIN 4
#define SERVO3_PIN 16
#define SERVO4_PIN 17
#define SERVO5_PIN 18


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo bigServo;

const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 3);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

void setup() {
  Serial.begin(115200);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  bigServo.attach(SERVO5_PIN);

  servo1.write(90); // 90 - inside; 0 - outside; 20-straight;
  servo2.write(90); 
  servo3.write(90); // 0 - straight; 
  servo4.write(90);
}

void loop() {
  // turn
  servo1.write(135); 
  servo2.write(45); 
  servo3.write(135); 
  servo4.write(45);
  forward(motor1, motor2, 150); 
  delay(3000);
  back(motor1, motor2, 150);
  delay(3000);

  // strainght
  servo1.write(90); 
  servo2.write(90); 
  servo3.write(90); 
  servo4.write(90);
  forward(motor1, motor2, 150); 
  delay(3000);
  back(motor1, motor2, 150);
  delay(3000);
}
