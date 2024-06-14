#include <IRremote.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>

// Pins for 8x8 LED Matrix
#define ROW_1 A4
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

int S1 = 2;
int s1State = 0;
bool flag = false;
int grootte = 5;
int row = 0;
byte display_row[8] = { B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111 };

float timeCount = 0;
long time = millis();
int delayTime = 10;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; 

// Pins for Ultrasone Sensor
const int trigPin = 22;
const int echoPin = 23;
const int ledPin = 52;   // LED indicator pin

// Pins for DFPlayer Mini
static const uint8_t PIN_MP3_TX = 18;
static const uint8_t PIN_MP3_RX = 19;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

//object voor de player
DFRobotDFPlayerMini player;

// IR Remote button codes
const unsigned long POWER_BUTTON_CODE = 0xBD42FF00;
const unsigned long VOLUME_UP_BUTTON_CODE = 0xB946FF00;
const unsigned long VOLUME_DOWN_BUTTON_CODE = 0xEA15FF00;
const unsigned long RESET_BUTTON_CODE = 0x49B0F624;

bool systemOn = true; // System state
int volume = 15; // Volume level, default 15 (schaal van 0-30)

// Define IR receiver pin
const int IR_RECEIVER_PIN = 50;
IRrecv irReceiver(IR_RECEIVER_PIN);

// Servo
Servo myservo;
int pos = 0;

void setup() {
  Serial.begin(9600);

  // Setup 8x8 LED Matrix
  const byte rows[] = { ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8 };
  const byte col[] = { COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8 };

  for (byte i = 0; i < 8; i++) {
    pinMode(rows[i], OUTPUT);
    pinMode(col[i], OUTPUT);
  }
  pinMode(S1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(S1), buttonPressed, FALLING);

  // Setup Ultrasone Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Setup DFPlayer Mini
  softwareSerial.begin(9600);
  if (player.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini initialized.");
    player.volume(volume); // Set default volume
  } else {
    Serial.println("Failed to initialize DFPlayer Mini.");
  }

  // Setup IR Receiver
  irReceiver.enableIRIn();

  // Setup Servo
  myservo.attach(24);
  myservo.write(pos);
}

void loop() {
  // Check for IR Remote commands
  if (irReceiver.decode()) {
    unsigned long receivedCode = irReceiver.decodedIRData.decodedRawData;

    if (receivedCode == POWER_BUTTON_CODE) {
      systemOn = !systemOn;
      Serial.println(systemOn ? "System ON" : "System OFF");

      if (!systemOn) {
        powerDownSystem();
      } else {
        powerUpSystem();
      }
    } else if (receivedCode == VOLUME_UP_BUTTON_CODE) {
      if (systemOn && volume < 30) { // Max volume is 30
        volume += 5; // Verhoog volume met sprongen van 5
        updateVolume();
      }
    } else if (receivedCode == VOLUME_DOWN_BUTTON_CODE) {
      if (systemOn && volume > 0) {
        volume -= 5; // Verlaag volume met sprongen van 5
        updateVolume();
      }
    } else if (receivedCode == RESET_BUTTON_CODE) {
      if (systemOn) {
        resetGame(); // Reset de game als systeem aan staat
      }
    }

    irReceiver.resume();
  }

  // TOWERGAME logic
  if (!systemOn) {
    return; // If system is off, do not run game logic
  }

  updateGameDisplay();
  checkUltrasoundSensor();
}

void updateGameDisplay() {
  // Game logic
  bool flag = false;

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
      time = millis();
      while (millis() - time < delayTime) {
        drawScreen();
      }
    }
  }
}

void drawScreen() {
  // Draw function for 8x8 LED Matrix
  const byte rows[] = { ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8 };
  const byte col[] = { COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8 };

  for (byte o = 0; o < 8; o++) {
    digitalWrite(rows[o], HIGH);
    for (byte a = 0; a < 8; a++) {
      digitalWrite(col[a], (display_row[o] >> a) & 0x01);
      delayMicroseconds(500);
      digitalWrite(col[a], 1);
    }
    digitalWrite(rows[o], LOW);
  }
}

void buttonPressed() {
  if (millis() - lastDebounceTime > debounceDelay){
    lastDebounceTime = millis();
    flag = true;

    int previousRow = row; // Houd de vorige waarde van row bij

    row++;

    if (row > 1) {
      display_row[row - 1] = (display_row[row - 2] | display_row[row - 1]);
      grootte = 0;
      for (int i = 0; i < 8; i++) {
        if (((display_row[row - 1] >> i) & 0x01) == 0) {
          grootte++;
        }
      }

      // Controleer of er een nieuw blok is geplaatst
      if (row > previousRow) {
        player.playMp3Folder(1); // Play MP3 file 0001.mp3 in folder "mp3"
      }

      // Controleer of de toren de top heeft bereikt
      if (row == 7) {
        openServo();
      }
    }
  }
}

void checkUltrasoundSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  
  if (distance > 0 && distance < 2000) { // Controleer op geldige afstandsmeting
    if (distance > 100 && distance < 800) {
      Serial.println("LED AAN");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED UIT");
      digitalWrite(ledPin, LOW);
    }
  } else {
    Serial.println("Foutieve meting, wordt genegeerd");
  }
}


void updateVolume() {
  // Update volume function for DFPlayer Mini
  player.volume(volume);
}

void powerDownSystem() {
  // Power down all pins except IR receiver pin
  for (int pin = 2; pin <= 53; pin++) {
    if (pin != IR_RECEIVER_PIN) {
      pinMode(pin, INPUT);
    }
  }
  for (int pin = A0; pin <= A15; pin++) {
    pinMode(pin, INPUT);
  }
  Serial.println("System is powered down.");
}

void powerUpSystem() {
  // Power up all pins
  for (int pin = 2; pin <= 53; pin++) {
    pinMode(pin, OUTPUT);
  }
  for (int pin = A0; pin <= A15; pin++) {
    pinMode(pin, OUTPUT);
  }
  setup(); // Reset game state
  Serial.println("System is powered up.");
}

void openServo() {
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);
    delay(5);
  }
  for (pos = 90; pos >= 0; pos--) {
    myservo.write(pos);
    delay(5);
  }
}

void resetGame() {
  grootte = 5; // Reset grootte naar oorspronkelijke waarde
  row = 0; // Reset row naar oorspronkelijke waarde
  for (int i = 0; i < 8; i++) {
    display_row[i] = B11111111; // Reset elke byte in display_row naar B11111111
  }
}
