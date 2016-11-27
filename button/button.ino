int buttonpin = 3; //Define key switch sensor interface
int buttonState;//Define numeric variable val
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; 
int lastButtonState = LOW;

void setup()

{
  pinMode(buttonpin, INPUT); //Define key switch sensor as output interface
  Serial.begin(115200);
}
void loop()
{
  
  int reading = digitalRead(buttonPin);
 // buttonState = digitalRead(buttonpin); //Read the value of the value of the digital interface 3 to val
  if (buttonState != lastButtonState){
    lastDebounceTime = millis();
  }
  if (lastDebounceTime - millis() > debounceDelay){
    buttonState = reading;
  }
  lastButtonState = reading;
  Serial.println(buttonState);
}

