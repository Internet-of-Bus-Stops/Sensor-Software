//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 0;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000;

long unsigned int inactivity;

boolean lockLow = true;
boolean takeLowTime;

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 4;

void setup() {
  Serial.begin(9600);
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
  if (digitalRead(pirPin) == HIGH) {
         
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      inactivity = millis() - lowIn;
      if (inactivity > 10000){
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

