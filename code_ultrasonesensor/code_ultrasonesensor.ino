const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 13; // Definieer een constante voor de LED pin

long duration;
int distance;
float taken_volume;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //zetten in het begin van de code
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  // Controleer op foutieve metingen
  if (distance > 0 && distance < 2000) { // Stel een maximumwaarde in die logisch is voor jouw toepassing
    volume = distance * 7.8;
    Serial.print("Volume: ");
    Serial.println(volume);
    
    if (volume > 100 && volume < 800) {
      Serial.println("LED AAN");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED UIT");
      digitalWrite(ledPin, LOW);
    }
  } else {
    Serial.println("Foutieve meting, wordt genegeerd");
  }
  
  delay(1000); // Voeg een korte vertraging toe om het gemakkelijker te maken de seriële uitvoer te lezen LATER VERWIJDEREN
}
