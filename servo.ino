#include <Servo.h>

Servo myservo;

int pos = 0;

void setup() {
  myservo.attach(9);
  myservo.write(pos);
  pinMode(8, INPUT_PULLUP);
}

void loop() {

  bool d = digitalRead(8);
  if (d == 0) {
    for (pos = 0; pos <= 90; pos ++ ) {
      myservo.write(pos);
      delay(5);
    }
      for (pos = 90; pos >= 0; pos -- ) {
      myservo.write(pos);
      delay(5);
    }
  }
}
