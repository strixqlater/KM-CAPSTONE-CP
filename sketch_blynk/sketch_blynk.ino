#define ESP8266_BAUD 115200 //115200

#define BLYNK_PRINT Serial

#define DHT_PIN 8
#define DHT_TYPE DHT11

#define MQ2_PIN A0

#define START_1 0x42
#define START_2 0x4d
#define DATA_LENGTH_H 0
#define DATA_LENGTH_L 1
#define PM1_0_ATMOSPHERE_H 8
#define PM1_0_ATMOSPHERE_L 9
#define PM2_5_ATMOSPHERE_H 10
#define PM2_5_ATMOSPHERE_L 11
#define PM10_ATMOSPHERE_H 12
#define PM10_ATMOSPHERE_L 13
#define PM2_5_PARTICLE_H 16
#define PM2_5_PARTICLE_L 17
#define VERSION 26
#define ERROR_CODE 27
#define CHECKSUM 29

#include <ESP8266_Lib.h>
#include <SoftwareSerial.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>

BlynkTimer timer;

char auth[] = "*";

char ssid[] = "*";
char pass[] = "*";

float h;
float t;

byte bytCount1 = 0;
byte bytCount2 = 0;

unsigned char chrRecv;
unsigned char chrData[30];

int PM01;
int PM25;
int PM10;

ESP8266 wifi(&Serial2);

DHT dht(DHT_PIN, DHT_TYPE);

unsigned int GetPM_Data(unsigned char chrSrc[], byte bytHigh, byte bytLow)
{
   return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];
}

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial2.begin(ESP8266_BAUD);
  delay(10);

  Serial3.begin(9600);
  delay(10);
  
  Blynk.begin(auth, wifi, ssid, pass);

  timer.setInterval(1000L, myTimerEvent1);
  timer.setInterval(1000L, myTimerEvent2);
  timer.setInterval(1000L, myTimerEvent3);
  timer.setInterval(1000L, myTimerEvent4);
  timer.setInterval(1000L, myTimerEvent5);
  timer.setInterval(1000L, myTimerEvent6);
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

void myTimerEvent4() { // PM0.1
  Blynk.virtualWrite(V4, PM01);
}

void myTimerEvent5() { // PM2.5
  Blynk.virtualWrite(V5, PM25);
}

void myTimerEvent6() { // PM10
  Blynk.virtualWrite(V6, PM10);
}

void loop() {
  Blynk.run();
  timer.run();
  dht_read();
  pms7003_read();
  Serial.println(PM01);
  Serial.println(PM25);
  Serial.println(PM10);
  Serial.println();
}

void dht_read() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  delay(100);
}

void pms7003_read() {
  if (Serial3.available()) {
       for(int i = 0; i < 32; i++) {
           chrRecv = Serial3.read();
           if (chrRecv == START_2 ) {
              bytCount1 = 2;
              break;
            }
       }
      if (bytCount1 == 2) {
        bytCount1 = 0;
        
        for(int i = 0; i < 30; i++){
          chrData[i] = Serial3.read();
        }
  
        if ((unsigned int) chrData[ERROR_CODE] == 0 ) {
          PM01  = GetPM_Data(chrData, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L);
          PM25  = GetPM_Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);
          PM10  = GetPM_Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);
          Serial.print("PM1.0=");
          Serial.print(PM01);
          Serial.print(",PM2.5=");
          Serial.print(PM25);
          Serial.print(",PM10=");
          Serial.println(PM10);
        }
        
        else {
          Serial.println("PMS7003  ERROR");
        }
      }
  }
  
  else{
    Serial.println("PMS7003 NOT available");
  }
}
