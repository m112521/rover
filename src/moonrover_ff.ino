/**
https://github.com/madhephaestus/ESP32Servo/
*/
#include <TB6612_ESP32.h>
#include <ESP32Servo.h>

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY

#define SERVO1_PIN 5
#define SERVO2_PIN 4
#define SERVO3_PIN 16
#define SERVO4_PIN 17
#define SERVO5_PIN 18 // release servo

// Avaible pins: 0, 19, 21, 22, TX, RX, 32, 34, 35
#define AIN12 17 // !
#define BIN12 4  // ! 
#define AIN22 5  // ! 
#define BIN22 2 
#define PWMA2 18 // ! 
#define PWMB2 15
#define STBY2 16 // ! 


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo bigServo;

const int offsetA = 1;
const int offsetB = 1;

int speed = 150;


Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 5); // 5000 - freq; 8 - resolution; 1 - channel
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 6);

void setup() {
  Serial.begin(115200);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  //bigServo.attach(SERVO5_PIN);

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
  motor1.drive(speed);
  motor2.drive(-speed);
  delay(3000);
  motor1.drive(-speed);
  motor2.drive(speed);
  delay(3000);

  // strainght
  servo1.write(90); 
  servo2.write(90); 
  servo3.write(90); 
  servo4.write(90);
  forward(motor1, motor2, speed); 
  delay(3000);
  back(motor1, motor2, speed);
  delay(3000);
}
