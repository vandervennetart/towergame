#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

static const uint8_t PIN_MP3_TX = 12;
static const uint8_t PIN_MP3_RX = 13;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

//object voor de player
DFRobotDFPlayerMini player;

void setup() {
  Serial.begin(9600);          //buadrate voor de audio player
  

  //testen of de player correct werkt
}

void loop() {
}
