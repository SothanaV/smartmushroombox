#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
////////////////////////////////////////////////////////////////////////////////
//////////Define Pin///////////////////////////////////////////////////////////
#define USE_SERIAL Serial
int DHTPIN = 2;          //setpin of DHT at D0
int DHTTYPE = DHT22;     //set Dht Type 
/////////////////-----------------------------------//////////////////////////
int Cooler = 4;             //setpin of Refrigeratoe at D2
int Pump = 5;               //setpin of pump to D1
int Led_Red = 14;            //setpin of Led Red at D5
int Led_Green = 12;            //setpin of Led Red at D6
int Led_Blue = 13;            //setpin of Led Red at D7
///////////////////////////////////////////////////////////////////////////////
void SendData(float h , float t);     //Define Senddata Voide
void OnCooler();
void OffCooler();
void OnPump();
void OffPump();
////////////Setup//////////////////////////////////////////////////////////////

const char* ssid     = "linksys_SES_54153";            //Set ssid
const char* password = "vasabilab";                    //Set Password
const char* Server   = "10.100.20.173";           //set Server Domain or Server ip
DHT dht(DHTPIN, DHTTYPE);                             //Start DHT
ESP8266WiFiMulti WiFiMulti;


void setup() 
{
    USE_SERIAL.begin(115200);
      for(uint8_t t = 6; t > 0; t--) 
      {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
      }
    WiFiMulti.addAP(ssid, password);    //Set SSID and Password (SSID, Password)
    WiFi.begin(ssid, password);         //Set starting for Wifi
    Serial.println(WiFi.localIP());
    dht.begin();

    ////////////////////////////////SetuoPinMode//////////////////////////////////////////
    pinMode(Led_Red, OUTPUT);
    pinMode(Led_Green, OUTPUT);
    pinMode(Led_Blue, OUTPUT);
    pinMode(Pump,OUTPUT);
    pinMode(Cooler,OUTPUT);
    
                                            
}

////////////////////Loop////////////////////////////////////////////////////////////////////

void loop() 
{
   float h = dht.readHumidity();      //Read Humidity
   float t = dht.readTemperature();   //Read Temperature
   //float t = 25;                                                  ////<<<<<<TEST VALUE
   //float h = 50;                                                  ////<<<<<<TEST VALUE
   Serial.print("TEMPERATURE");
   Serial.println(t);
   Serial.print("HUMIDITY");
   Serial.println(h);
   if (isnan(t) || isnan(h)) 
  {
    Serial.println("FaiLed to read from DHT");
  } 
  else 
  {
    SendData(h,t);
  }

}

///////////////////////SednData//////////////////////////////////////////////////////////////////
void SendData(float h,float t) 
{
  
  // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;
        String str = "http://" +String(Server)+":5000" +"/data/" + String(t)+"/"+String(h);
        Serial.println(str);
        http.begin(str);
        int httpCode = http.GET();
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode > 0) 
        {
            if(httpCode == HTTP_CODE_OK) 
              {
                String payload = http.getString();
      //-------------------Control----------------------------//
                USE_SERIAL.print("payload");
                USE_SERIAL.println(payload);
                String Red = payload.substring(8,11);
                String Green = payload.substring(12,15);
                String Blue = payload.substring(16,19);
                int v_red = Red.toInt();
                int v_green = Green.toInt();
                int v_blue = Blue.toInt();
                Serial.println("Value Led Red");
                Serial.println(v_red);
                  if(payload.substring(0,3)=="101")
                    {
                     //OnCooler;
                     digitalWrite(Cooler,HIGH);
                     Serial.println("OnCooler");
                    }
                   if(payload.substring(4,7)=="201")
                    {
                      //OnPump;
                      digitalWrite(Pump,HIGH);
                      Serial.println("OnPump");                    
                    }
                   if(payload.substring(0,3)=="100")
                    {
                      //OffCooler;
                      digitalWrite(Cooler,LOW);
                      Serial.println("OffCooler");
                    }
                   if(payload.substring(4,7)=="200")
                    {
                      //OffPump;
                      digitalWrite(Pump,LOW);
                      Serial.println("OffPump");
                    }
                  analogWrite(Led_Red,v_red);
                  analogWrite(Led_Green,v_green);
                  analogWrite(Led_Blue,v_blue);
              }
        }
        http.end();
    }
    delay(500);
}

////////////////OnCooler//////////////////////////////////////////////////////////////

void Oncooler()
{
  digitalWrite(Cooler,HIGH);
  //delay(100);                   //Set Delay for Refrigterator
}

/////////////////OffCooler//////////////////////////////////////////////////////////////

void OffCooler()
{
  digitalWrite(Cooler,LOW);
  delay(100);
}

//////////////////OnPump//////////////////////////////////////////////////////////////////

void OnPump()
{
  digitalWrite(Pump,HIGH);
}
////////////////OffPump///////////////////////////////////////////////////////////////////

void OffPump()
{
  digitalWrite(Pump,LOW);
}


