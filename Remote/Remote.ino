#include <IRremote.h>

const int RECV_PIN = 11;
const int SPEAKER_PIN = 12;

const unsigned long POWER_BUTTON_CODE = 0xBD42FF00;
const unsigned long VOLUME_UP_BUTTON_CODE = 0xB946FF00;
const unsigned long VOLUME_DOWN_BUTTON_CODE = 0xEA15FF00;

bool systemOn = true; // System state
int volume = 5; // volume level start (schaal assumptie 0-10)

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN);
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    if (receivedCode == POWER_BUTTON_CODE) {
      systemOn = !systemOn;
      Serial.println(systemOn ? "System ON" : "System OFF");

      if (!systemOn) {
        powerDownSystem();
      } else {
        powerUpSystem();
      }
    } else if (receivedCode == VOLUME_UP_BUTTON_CODE) {
      if (systemOn && volume < 10) {
        volume++;
        Serial.print("Volume Up: ");
        Serial.println(volume);
        analogWrite(SPEAKER_PIN, map(volume, 0, 10, 0, 255));
      }
    } else if (receivedCode == VOLUME_DOWN_BUTTON_CODE) {
      if (systemOn && volume > 0) {
        volume--;
        Serial.print("Volume Down: ");
        Serial.println(volume);
        analogWrite(SPEAKER_PIN, map(volume, 0, 10, 0, 255));
      }
    }

    IrReceiver.resume();
  }

  if (systemOn) {
    // Alle code komt hierin

    //
  }
}

// Systeem UIT door alle pins uit te zetten (buiten de receiverpin)
void powerDownSystem() {
  for (int pin = 2; pin <= 13; pin++) {
    if (pin != RECV_PIN) {
      pinMode(pin, INPUT);
    }
  }
  for (int pin = A0; pin <= A5; pin++) {
    pinMode(pin, INPUT);
  }
  Serial.println("System is powered down.");
}

// Systeem AAN door alle pins aan te zetten
void powerUpSystem() {
  for (int pin = 2; pin <= 13; pin++) {
    if (pin != RECV_PIN) {
      pinMode(pin, OUTPUT);
    }
  }
  for (int pin = A0; pin <= A5; pin++) {
    pinMode(pin, OUTPUT);
  }
  setup(); // Reset de game state
  Serial.println("System is powered up.");
}
