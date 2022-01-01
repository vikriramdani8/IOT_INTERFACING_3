#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h>

// Uno to ESP8266
SoftwareSerial ArduinoUno (9,8);

// Sensor DHT
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor Gempa
int vibration_Sensor = 10;
int present_condition = 0;
int previous_condition = 0;

// Sensor Api
const int sensorMin = 0;
const int sensorMax = 1024;
#define FLAMEPIN 11

// Sensor tanah
const int SoilSensor = 9;
int sensorValue = 0;
int humidity = 0;

// Lamp1`
#define LAMP1 6
#define LAMP2 5
#define LAMP3 4
#define LAMP4 3
#define LAMP5 2

void setup() {
  Serial.begin(19200);
  ArduinoUno.begin(4800);
  
  pinMode(9, INPUT);
  pinMode(vibration_Sensor, INPUT);
  pinMode(FLAMEPIN, INPUT);
  pinMode(LAMP1, OUTPUT);
  pinMode(LAMP2, OUTPUT);
  pinMode(LAMP3, OUTPUT);
  pinMode(LAMP4, OUTPUT);
  pinMode(LAMP5, OUTPUT);
  dht.begin();
}

unsigned long lastMsg1 = 0;
unsigned long lastMsg2 = 0;
unsigned long bencanaTimes = 0;
unsigned long bencanaBlinkTimes = 0;
bool bencana = false;
bool bencanaState = LOW;

void loop() {
    unsigned long now = millis();

    if(bencana){
      if(now - bencanaBlinkTimes > 250){      
        if (bencanaState == LOW) {
          bencanaState = HIGH;
        } else {
          bencanaState = LOW;
        }

        digitalWrite(LAMP5, bencanaState);
        bencanaBlinkTimes = now;  
      }
      
      if (now - bencanaTimes > 20000) {
        bencana = false;
      }
    } else {
      digitalWrite(LAMP5, LOW);  
      bencanaTimes = 0;
      bencanaBlinkTimes = 0;
    }
  
    
    if (now - lastMsg1 > 1000) {
      previous_condition = present_condition;
      present_condition = digitalRead(vibration_Sensor);

      if (previous_condition != present_condition) {
        Serial.println("Gempa");
        ArduinoUno.println(211);
        bencana = true;
        bencanaTimes = now;
      }

      int state = digitalRead(FLAMEPIN);
      if(state == LOW){
        Serial.println("Api terdeteksi");
        ArduinoUno.println(311);
        bencana = true;
        bencanaTimes = now;
      }
      
      lastMsg1 = now;
    }

    while(ArduinoUno.available() > 0){
        int val = ArduinoUno.parseInt();
        Serial.println(val);

        switch(val){
          case 110:
            digitalWrite(LAMP1, LOW);
          break;
          case 111:
            digitalWrite(LAMP1, HIGH);
          break;
          case 120:
            digitalWrite(LAMP2, LOW);
          break;
          case 121:
            digitalWrite(LAMP2, HIGH);
          break;
          case 130:
            digitalWrite(LAMP3, LOW);
          break;
          case 131:
            digitalWrite(LAMP3, HIGH);
          break;
          case 140:
            digitalWrite(LAMP4, LOW);
          break;
          case 141:
            digitalWrite(LAMP4, HIGH);
          break;
        }
//        ArduinoUno.print(202);
    }
  
    if (now - lastMsg2 > 5000) {

      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
      } else {
        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("C ")); 
        Serial.println();
        ArduinoUno.print(t);
      }

      lastMsg2 = now;
    }
}

int convertToPercent(int value){
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}
