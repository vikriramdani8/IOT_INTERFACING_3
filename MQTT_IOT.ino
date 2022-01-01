#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <FirebaseESP8266.h>
#include <WiFiClientSecure.h>

FirebaseData fbdo;
SoftwareSerial NodeMCU(D2, D1);
boolean parsing=false;
int commandLamp = 0;

#define pinDHT 5
#define DHTType DHT11

DHT dht(pinDHT, DHTType);

#define FIREBASE_HOST "https://iot-stupidhome-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "5UvcLRHQu1fSEUcmOtbKsUdmn0dx5pb6D2jT11oT"
const char* ssid = "-"; //--> Nama Wifi / SSID.
const char* password = "vikriramdani";
const char* mqtt_server = "192.168.1.5";

const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbwpSfonhea1FQ2gWZh_8TMBoNiqI8CyBbiGCtoCKTMN_ZP4Iea93gCVvfNTe70d_i6P"; //--> spreadsheet script ID

WiFiClient espClient;
WiFiClientSecure client2;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  }
   
  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH);
  }

  if(topic == "svgeng/nodemcu/lamp1"){
    if((char)payload[0] == '1'){
      NodeMCU.print(111);
    } else {
      NodeMCU.print(110);
    }
  } else if(topic == "svgeng/nodemcu/lamp2"){
    if((char)payload[0] == '1'){
      NodeMCU.print(121);
    } else {
      NodeMCU.print(120);
    }
  } else if(topic == "svgeng/nodemcu/lamp3"){
    if((char)payload[0] == '1'){
      NodeMCU.print(131);
    } else {
      NodeMCU.print(130);
    }
  } else if(topic == "svgeng/nodemcu/lamp4"){
    if((char)payload[0] == '1'){
      NodeMCU.print(141);
    } else {
      NodeMCU.print(140);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect("local-broker")) {
      Serial.println("connected");

      client.subscribe("svgeng/nodemcu/lamp1");
      client.subscribe("svgeng/nodemcu/lamp2");
      client.subscribe("svgeng/nodemcu/lamp3");
      client.subscribe("svgeng/nodemcu/lamp4");

      client.subscribe("svgeng/nodemcu/gempa");
      client.subscribe("svgeng/nodemcu/kebakaran");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  NodeMCU.begin(4800);
  pinMode(D2, INPUT);
  pinMode(D1, OUTPUT);
  
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//  client2.setInsecure();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  float temp = NodeMCU.parseFloat();
  if(temp > 10 && temp < 50){
    Firebase.setFloat(fbdo, "Suhu", temp);
//    sendData(temp);
  }
  
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
        
    lastMsg = now;
    ++value;

    client.publish("svgeng/nodemcu/suhu", "90 C");
  }
}

void sendData(float value) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client2.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String temp =  String(value, 2);
  String url = "/macros/s/" + GAS_ID + "/exec?nilai=" + temp;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client2.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client2.connected()) {
    String line = client2.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  
  String line = client2.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
} 
