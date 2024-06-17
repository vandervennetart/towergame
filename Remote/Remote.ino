#include <IRremote.h>

const int RECV_PIN = 11;

const unsigned long POWER_BUTTON_CODE = 0xBD42FF00;
const unsigned long VOLUME_UP_BUTTON_CODE = 0xB946FF00;
const unsigned long VOLUME_DOWN_BUTTON_CODE = 0xEA15FF00;


void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  IrReceiver.enableIRIn();

}

void loop() {
  
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    Serial.println(receivedCode);

    if (receivedCode == POWER_BUTTON_CODE) {
      Serial.println("power");

      
    } else if (receivedCode == VOLUME_UP_BUTTON_CODE) {

        Serial.print("Volume Up: ");
      
    } else if (receivedCode == VOLUME_DOWN_BUTTON_CODE) {

        Serial.print("Volume Down: ");
        
      
    }

    IrReceiver.resume();
  }
}
