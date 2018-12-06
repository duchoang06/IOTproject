#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "BH1750.h"
#include <Wire.h>

WiFiClient espclient;
BH1750 Light;

const int port = 8080;
const char host[] = "192.168.43.235";
//const char host[] = "192.168.43.140";
const char* ssid = "Cuder";
//char* pw = "841658193590";

unsigned long previousMillis = 0;
long interval = 2000;

void setup() 
{
  Serial.begin(115200);
  Wire.begin(D3,D4); //SDA, SCL
  Light.begin();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  if (!espclient.connect(host, port)) 
  {
    Serial.println("Connection to host failed") ;
    delay(500);
    return;
  }
  Serial.println("Connected to server successful!");
}

void loop() 
{
  if(millis() - previousMillis > interval)
  {
    previousMillis = millis();
    float lux = Light.readLightLevel();
    Serial.println("light: " + String(lux));
    espclient.println("light: " + String(lux));
  }
 
  if(!espclient.connected())
  {
    espclient.connect(host, port);
  }
}
