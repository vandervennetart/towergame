// code based on https://projecthub.arduino.cc/SAnwandter1/programming-8x8-led-matrix-a3b852

//update from SAnwandter

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

void setup() {
  // Open serial port
  Serial.begin(9600);

  // Set all used pins to OUTPUT
  // This is very important! If   the pins are set to input
  // the display will be very dim.
  for (byte i = 0; i <= 8; i++)
    pinMode(rows[i], OUTPUT);
  for (byte i = 0; i <= 8; i++)
    pinMode(col[i], OUTPUT);

  pinMode(S1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(S1), buttonPressed, FALLING);  // trigger when button pressed, but not when released.
}

void loop() {
  flag = false;
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
      Serial.println(flag); //niet verwijderen, ik heb geen idee waarom, maar zonder dit werkt het niet lol
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
  if (millis() - lastDebounceTime > debounceDelay){
    lastDebounceTime = millis();
    flag = true;
    
    row++;

    if (row>1){
      display_row[row-1] = (display_row[row-2] | display_row[row-1]);
      grootte = 0;
      for (int i=0; i<8; i++){
        if(((display_row[row-1] >> i) & 0x01) == 0){
          grootte++;
        }
      }
    }
  }
  
}
