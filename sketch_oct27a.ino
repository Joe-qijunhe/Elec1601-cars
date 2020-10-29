#include <Servo.h>
Servo servoLeft;
Servo servoRight;
void setup() {
  // put your setup code here, to run once:
  pinMode(10, INPUT);
  pinMode(9, OUTPUT);
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  servoLeft.attach(13);
  servoRight.attach(12);

  int irLeft = irDetect(9, 10, 38000);
  int irRight = irDetect(2, 3, 38000);
//  Serial.println(irLeft);
//  Serial.println(irRight);
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
  
  if (irLeft == 0 && irRight != 0) {
    servoLeft.writeMicroseconds(1700);
    servoRight.writeMicroseconds(1500);
    delay(1000);
  } else if (irRight == 0 && irLeft != 0) {
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1300);
    delay(1000);
  }
}
int irDetect(int irLedPin, int irReceiverPin  , long frequency) {
  tone(irLedPin, frequency, 0);
  delay(1);
  int ir = digitalRead(irReceiverPin);
  delay(1);
  return ir;
}
