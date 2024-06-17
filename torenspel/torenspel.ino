// code based on https://projecthub.arduino.cc/SAnwandter1/programming-8x8-led-matrix-a3b852
#include <IRremote.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>
#include "IRremote.hpp"


static const uint8_t PIN_MP3_TX = 12;
static const uint8_t PIN_MP3_RX = 13;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);



//object voor de player
DFRobotDFPlayerMini player;

//update from SAnwandter

#define ROW_1 38
#define ROW_2 45
#define ROW_3 30
#define ROW_4 37
#define ROW_5 50
#define ROW_6 31
#define ROW_7 51
#define ROW_8 33

#define COL_1 44
#define COL_2 52
#define COL_3 53
#define COL_4 36
#define COL_5 32
#define COL_6 39
#define COL_7 46
#define COL_8 47

bool spelBezig = true;

int S1 = 2;

int s1State = 0;
bool flag = false;
int grootte = 5;
int row = 0;

bool punt = false;

int i = 0;

byte display_row[8] = { B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111 };
const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte col[] = {
  COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

float timeCount = 0;
long time = millis();
int delayTime = 10;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;


//sensor
const int trigPin = 23;
const int echoPin = 22;
const int ledPin = 24;  // Definieer een constante voor de LED pin

long duration;
int distance;
float taken_volume;

Servo myservo;
int pos = 0;
bool servoOpen = false;

// IR Remote button codes
const unsigned long POWER_BUTTON_CODE = 0xBD42FF00;
const unsigned long VOLUME_UP_BUTTON_CODE = 0xB946FF00;
const unsigned long VOLUME_DOWN_BUTTON_CODE = 0xEA15FF00;
const unsigned long RESET_BUTTON_CODE = 0x49B0F624;
int volume = 30;  // Volume level, default 15 (schaal van 0-30)

// Define IR receiver pin
const int IR_RECEIVER_PIN = 11;
IRrecv irReceiver(IR_RECEIVER_PIN);

void setup() {
  

  spelBezig = true;
  // Open serial port
  Serial.begin(9600);

  softwareSerial.begin(9600);  //Maakt gebruik van softwareserial, heb dit gedaan omdat de normale serial IO wrs al gebruikt gaat worden voor een ander component.

  if (player.begin(softwareSerial)) {
    Serial.println("OK");
    player.volume(volume);

  } else {  //indien connectie niet goed is, wordt dit aangegeven op de serial monitor.
    Serial.println("Connecting to DFPlayer Mini failed!");
  }


  // Set all used pins to OUTPUT
  // This is very important! If   the pins are set to input
  // the display will be very dim.
  for (byte i = 0; i <= 8; i++)
    pinMode(rows[i], OUTPUT);
  for (byte i = 0; i <= 8; i++)
    pinMode(col[i], OUTPUT);

  pinMode(S1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(S1), buttonPressed, FALLING);  // trigger when button pressed, but not when released.

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Setup Servo
  myservo.attach(9);
  myservo.write(pos);

  // Setup IR Receiver
  irReceiver.enableIRIn();

  meetPrijzen();
}

void loop() {

  checkAfstandsbediening();

  while (spelBezig) {
    flag = false;
    checkAfstandsbediening();

    gameLogica();
    
  }
}


void gameLogica() {
  if (punt) {
    player.playMp3Folder(1);
    punt = false;
  }
  if (row == 7 && !servoOpen) {
      Serial.println("servo");
      openServo();
      resetGame();
    }

  for (int i = 0; i < grootte + 8; i++) {
    if (!flag) {
      if (i < grootte) {
        display_row[row] = display_row[row] << 1;

      } else {
        byte x = B11111111;

        display_row[row] = ((display_row[row] << 1) | (x >> (7 - (i - grootte))));
      }
      time = millis();
      while (millis() - time < delayTime) {
        drawScreen();
      }
    }
  }

  for (int i = grootte + 8; i > 0; i--) {
    if (!flag) {
      if (i > 8) {
        display_row[row] = display_row[row] >> 1;

      } else {
        byte x = B11111111;

        display_row[row] = ((display_row[row] >> 1) | (x << (i - 1)));
      }
      delay(1);
      // Serial.println(flag); //niet verwijderen, ik heb geen idee waarom, maar zonder dit werkt het niet lol
      time = millis();
      while (millis() - time < delayTime) {
        drawScreen();
      }
    }
  }
}

void drawScreen() {
  // Turn on each   row in series
  for (byte o = 0; o < 8; o++)  // count next row
  {
    digitalWrite(rows[o], HIGH);  //initiate whole row
    for (byte a = 0; a < 8; a++)  // count next row
    {
      // if You set   (~buffer2[i] >> a) then You will have positive
      digitalWrite(col[a], (display_row[o] >> a) & 0x01);  // initiate whole column

      delayMicroseconds(500);  // uncoment deley for diferent speed of display
      //delayMicroseconds(1000);
      //  delay(1);

      digitalWrite(col[a], 1);  // reset whole column
    }

    digitalWrite(rows[o], LOW);  // reset whole row
                                 // otherwise last row will intersect with next   row
  }
}

void buttonPressed() {


  if (millis() - lastDebounceTime > debounceDelay && !flag) {
    flag = true;
    lastDebounceTime = millis();

    Serial.println("a");



    punt = true;

    row++;

    if (row > 1) {
      display_row[row - 1] = (display_row[row - 2] | display_row[row - 1]);
      grootte = 0;
      for (int i = 0; i < 8; i++) {
        if (((display_row[row - 1] >> i) & 0x01) == 0) {
          grootte++;
        }
      }
    }


    

    if(grootte == 0){
      resetGame();
    }
  }
}

void meetPrijzen() {
  //zetten in het begin van de code
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  Serial.println(duration);
  // Controleer op foutieve metingen
  if (distance > 0 && distance < 2000) {  // Stel een maximumwaarde in die logisch is voor jouw toepassing
    long volume = distance * 7.8;
    Serial.print("Volume: ");
    Serial.println(volume);

    if (volume == 156) {
      Serial.println("LED AAN");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED UIT");
      digitalWrite(ledPin, LOW);
    }
  } else {
    Serial.println("Foutieve meting, wordt genegeerd");
  }

  delay(1000);  // Voeg een korte vertraging toe om het gemakkelijker te maken de seriële uitvoer te lezen LATER VERWIJDEREN
}

void openServo() {

  servoOpen = true;
  myservo.write(180);
  delay(5000);
  myservo.write(0);
}


void checkAfstandsbediening() {
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    Serial.println(receivedCode);

    if (receivedCode == POWER_BUTTON_CODE) {
      Serial.println("power");

      
    } else if (receivedCode == VOLUME_UP_BUTTON_CODE) {

        Serial.print("Volume Up: ");
        volume += 1;
        updateVolume();
      
    } else if (receivedCode == VOLUME_DOWN_BUTTON_CODE) {

        Serial.print("Volume Down: ");
        volume -= 1;
        updateVolume();
      
    }

    IrReceiver.resume();
  }
}


void updateVolume() {
  // Update volume function for DFPlayer Mini
  Serial.println(volume);
  player.volume(volume);
}

void powerDownSystem() {
  // Turn off 8x8 LED Matrix
  spelBezig = false;

  // Disable DFPlayer Mini
  // player.disable(); // Disable DFPlayer Mini module

  Serial.println("System is powered down.");
}

void powerUpSystem() {
  setup();  // Reinitialize pins and components

  Serial.println("System is powered up.");
}

void resetGame() {
  grootte = 5;  // Reset grootte naar oorspronkelijke waarde
  row = 0;      // Reset row naar oorspronkelijke waarde
  for (int i = 0; i < 8; i++) {
    display_row[i] = B11111111;  // Reset elke byte in display_row naar B11111111
  }
  spelBezig = true;
}
