#define BLYNK_PRINT Serial
#define DHT_PIN 8
#define DHT_TYPE DHT11
#define MQ2_PIN A0

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>

BlynkTimer timer;

char auth[] = "65bb14d27fce4e81ac0e4cace9e20e1b";

char ssid[] = "RT-AC1900P-2.4GHz";
char pass[] = "icnxkmaster";

float h;
float t;
float g;

#define ESP8266_BAUD 115200 //115200
ESP8266 wifi(&Serial);

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.begin(ESP8266_BAUD);
  delay(10);
 
  Blynk.begin(auth, wifi, ssid, pass);
  timer.setInterval(1000L, myTimerEvent1);
  timer.setInterval(1000L, myTimerEvent2);
  timer.setInterval(1000L, myTimerEvent3);
}

void myTimerEvent1() { // temp
  Blynk.virtualWrite(V1, t);
}

void myTimerEvent2() { // humid
  Blynk.virtualWrite(V2, h);
}

void myTimerEvent3() { // gas
  int g = analogRead(MQ2_PIN);
  Blynk.virtualWrite(V3, g);
}

void loop() {
  Blynk.run();
  timer.run();
  dht_read();
}

void dht_read() {
  t = dht.readTemperature();
  h = dht.readHumidity();
}
