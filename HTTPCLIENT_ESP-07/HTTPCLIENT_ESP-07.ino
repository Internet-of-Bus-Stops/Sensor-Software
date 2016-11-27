

#include <Arduino.h>
#include <DHT.h>;
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

//Constants
#define BUS_ID 2
#define DHTPIN 14     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
#define USE_SERIAL Serial

//Variables
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 0;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000;

long unsigned int inactivity;

long unsigned int inactiveThreshold = 20000;

boolean lockLow = true;
boolean takeLowTime;

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 4;
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

ESP8266WiFiMulti WiFiMulti;

void setup() {

    
    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("Junction", "");
    dht.begin();
    pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);

}

void loop() {

        hum = random(40,60);
        temp= random(20, 30);
        //Print temp and humidity values to serial monitor
        Serial.print("Humidity: ");
        Serial.print(hum);
        Serial.print(" %, Temp: ");
        Serial.print(temp);
        Serial.println(" Celsius");
        delay(1000);
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
  
        String humidity = "http://85.188.10.135:8000/sensor?stype=humidity";
        humidity +="&svalue=";
        humidity += hum;
        humidity +="&busid=";
        humidity += BUS_ID;
        humidity +="&rawvalue=3";
        Serial.println(humidity);
        
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(humidity); //HTTP
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();

        String temp = "http://85.188.10.135:8000/sensor?stype=temp";
        temp +="&svalue=";
        temp += temp;
        temp +="&busid=";
        temp += BUS_ID;
        temp +="&rawvalue=3";
        Serial.println(temp);
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(temp); //HTTP
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

void readPIR(){
  if (digitalRead(pirPin) == HIGH) {
         
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      inactivity = millis() - lowIn;
      if (inactivity > inactiveThreshold){
        //send to database: bus stop is inactive
      }
      else {
        //do nothing
      }
      
        /*  if (inactivity > 1000){
            Serial.print("inactivity time = ");
            Serial.println(inactivity/1000);
         }*/
      lockLow = false;
      digitalWrite(ledPin, HIGH);
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec");
      delay(50);
      Serial.println("");
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW) {
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
              takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause) {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      digitalWrite(ledPin, LOW);
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      delay(50);
      Serial.println("");
    }
  }
}

