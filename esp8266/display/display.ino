#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <FirebaseArduino.h>

#define init0 0
#define getData 1
#define controlLight 2
#define displayLcd 3

#define FIREBASE_HOST "weathertracking-1803.firebaseio.com"
#define FIREBASE_AUTH ""

LiquidCrystal_I2C lcd(0x27, 16, 2);
int state;
String control = "OFF";
String temp = "";
String light = "";
String humid = "";

void gettingData() {
    temp = Firebase.getString("currentValue/currentTemp");
    humid = Firebase.getString("currentValue/currentHumid");
    light = Firebase.getString("currentValue/currentLight");
}

void initThings() {
    Serial.begin(115200);
    WiFi.begin("Cuder");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Wire.begin(D1,D2); // SDA, SCL
    lcd.init();   
    lcd.backlight();
    lcd.clear();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    pinMode(D5, OUTPUT);
}

void handleLight() {
    if (light.toFloat() <= Firebase.getString("Threshold").toFloat()) {
        digitalWrite(D5, HIGH);
        Firebase.setString("Control", "1");
        control = "ON";
    } else {
        digitalWrite(D5, LOW);
        Firebase.setString("Control", "0");
        control = "OFF";
    }
}

void displayLCD() {
    lcd.clear();
    lcd.print("H: " + humid + "%  T: " +temp+(char)223+"C");
    lcd.setCursor(0, 1);
    lcd.print("L: " + String(light.toInt()) + " - " + control);
}

void setup() {
    state = init0;
}

void loop() {
    switch(state) {
        case init0: {
            initThings();
            state = getData;
        }
        case getData: {
            gettingData();
            state = controlLight;
        }
        case controlLight: {
            handleLight();
            state = displayLcd;
        }
        case displayLcd: {
            displayLCD();
            state = getData;
        }
    }
    delay(100);
}

