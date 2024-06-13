#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

static const uint8_t PIN_MP3_TX = 2;
static const uint8_t PIN_MP3_RX = 3;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

//object voor de player
DFRobotDFPlayerMini player;

void setup() {
  Serial.begin(9600); //buadrate voor de audio player
  softwareSerial.begin(9600);//Maakt gebruik van softwareserial, heb dit gedaan omdat de normale serial IO wrs al gebruikt gaat worden voor een ander component.
  int previousRow = 0; // Nieuwe variabele om de vorige waarde van row bij te houden


  //testen of de player correct werkt
  if (player.begin(softwareSerial)) {
    Serial.println("OK");

    if (row > previousRow) {
      player.volume(30)
      player.playMp3Folder(1); // speeld een winnend geluids effect af. Moet in een folder "mp3" en met "0001.mp3" als bestandsnaam
      previousRow = row; // Werk de vorige row waarde bij
    }

  } else { //indien connectie niet goed is, wordt dit aangegeven op de serial monitor.
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

