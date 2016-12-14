#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial
#include "DHT.h"
#define DHTPIN 2          //setpin of DHT at D4
#define DHTTYPE DHT22     //set Dht Type 
#define FADESPEED 5
int Fan = 15;             //setpin of RelayFan at D8
int Peltier1 = 12;         //setpin of RelayPeltier at D7 //13
int Peltier2 = 14;        //setpin of RelayPeltier2 st D6 //12
int Pump = 1;            //setpin of pump to D5
int LED_Red = 0;          //setpin of LED Red at D3
int LED_Green = 4;        //setpin of LED Green at D2
int LED_Blue = 5;         //setpin of LED Blue at D1
int REDPIN = LED_Red;
int GREENPIN = LED_Green;
int BLUEPIN = LED_Blue;

DHT dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti WiFiMulti;
void sendAlarm(float h , float t);     //Define Senddata Voide
void setup() 
{
    pinMode(4, INPUT);
    USE_SERIAL.begin(115200);
    for(uint8_t t = 6; t > 0; t--) 
     {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
    WiFiMulti.addAP("SmartMushroomBox" ,"12345678");    //Set SSID and Password (SSID, Password)
    WiFi.begin();                     //Set starting for Wifi
    Serial.println(WiFi.localIP());
    pinMode(Fan,OUTPUT);              //set pinMode for fan output
    pinMode(Peltier1,OUTPUT);          //set pinMode for peltier output
    pinMode(Peltier2,OUTPUT);          //set pinMode for peltier output
    pinMode(Pump,OUTPUT);             //set pinMode for Pump output
    dht.begin();                      //set starting for DHT
    pinMode(LED_Red, OUTPUT);
    pinMode(LED_Green, OUTPUT);
    pinMode(LED_Blue, OUTPUT);
    
}
void led(int r,int g,int b)
{
  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
}
void loop() 
{
   float h = dht.readHumidity();      //Read Humidity
   float t = dht.readTemperature();   //Read Temperature
   Serial.print("TEMPERATURE");
   Serial.println(t);
   Serial.print("HUMIDITY");
   Serial.println(h);
   if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else 
  {
  }
   int r, g, b;  
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) 
  { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) 
  { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) 
  { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) 
  { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  for (g = 255; g > 0; g--) 
  { 
  analogWrite(GREENPIN, g);
  delay(FADESPEED);
  }
  digitalWrite(Fan,HIGH);
  delay(10);
  digitalWrite(Peltier1,HIGH);
  delay(10);
  digitalWrite(Peltier2,HIGH);
  delay(10);
}

void sendAlarm(float h,float t) 
{
  
  // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;
        String str = "http://172.20.10.11:5000/data/" + String(t)+"/"+String(h);
        Serial.println(str);
        http.begin(str);
        int httpCode = http.GET();
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode > 0) 
        {
            if(httpCode == HTTP_CODE_OK) 
              {
                String payload = http.getString();
                USE_SERIAL.println(payload);
              }
        }
        http.end();
    }
    delay(2000);
}

