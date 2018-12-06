#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "dht.h"

WiFiClient espclient;
dht DHT1;

const int port = 8080;
const char host[] = "192.168.43.235";
const char* ssid = "Cuder";
//char* pw = "841658193590"

float t;
float h;
unsigned long previousMillis = 0;
long interval = 3000;

void setup()
{
  pinMode(D1,INPUT);
  Serial.begin(115200);
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
    int sensor = DHT1.read11(D1);
    t = DHT1.temperature;
    h = DHT1.humidity;
    if (isnan(h) || isnan(t)) 
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.println("temp: " + String(t));
    espclient.println("temp: " + String(t));
    delay(1000);
    Serial.println("humid: " + String(h));
    espclient.println("humid: " + String(h));
  }
  
  if(!espclient.connected())
  {
    espclient.connect(host,port);
  }
}


