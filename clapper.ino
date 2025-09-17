#include <IRremote.h>

bool isLightOn = false;

const int RELAY_CONTROL_PIN = 2;

const int IR_POWER = 4;
const int IR_SIGNAL = 5;

const int MIC_POWER = 6;
const int MIC_SIGNAL = 7; 

// IR remote variables
int irPressCount = 0;
unsigned long lastIrPressTime = 0;
unsigned long specialLightEndTime = 0;
bool inSpecialTimeStart = false;  
bool turningOn;

// Microphone/clap detection variables
int clapCount = 0;
unsigned long lastClapTime = 0;

void turnLightOff() {
  isLightOn = false;
  digitalWrite(RELAY_CONTROL_PIN, LOW);
  Serial.println("Light turning off");
}

void turnLightOn() {
  isLightOn = true;
  turningOn = true; 
  digitalWrite(RELAY_CONTROL_PIN, HIGH);
  delay(750);
  turningOn = false; 
  Serial.println("Light turning on");
}

void toggleLight() {
  if (isLightOn) {
    turnLightOff();
  } else {
    turnLightOn();
  }
}

void setup() {
  pinMode(RELAY_CONTROL_PIN, OUTPUT);

  // Setup power outputs for sensors
  pinMode(IR_POWER, OUTPUT);
  pinMode(MIC_POWER, OUTPUT);

  // Setup sensor inputs
  pinMode(MIC_SIGNAL, INPUT);
  IrReceiver.begin(IR_SIGNAL, ENABLE_LED_FEEDBACK);

  Serial.begin(9600);

  // Start with light off
  turnLightOff();
}

void loop() {
  digitalWrite(IR_POWER, HIGH);
  digitalWrite(MIC_POWER, HIGH);

  unsigned long now = millis();

  //IR input
  if (!turningOn && IrReceiver.decode()) {
    // If more than 1 second has passed since the last IR press, reset the counter
    if (now - lastIrPressTime > 1000) {
      irPressCount = 0;
    }
    irPressCount++;
    lastIrPressTime = now;
    Serial.print("IR received, count: ");
    Serial.println(irPressCount);
    delay(250);
    IrReceiver.resume(); // Ready for next code
  }
  
  // If we've had an IR press but no new press in over 1 second, decide on action.
  if (irPressCount > 0 && (now - lastIrPressTime > 1000)) {
    if (irPressCount == 3) {
      inSpecialTimeStart = true; 
      // on 3 presses set special time half hour away 
      IrReceiver.stop();
      turnLightOn();
      delay(1000);
      turnLightOff();
      delay(1000);
      turnLightOn();
      Serial.println("Turning on for 30");
      specialLightEndTime = now + 30UL * 60UL * 1000UL;  
      delay(1000);
      inSpecialTimeStart = false; 
      IrReceiver.start();
    } else {
      // For one (or two) presses, toggle the light.
      Serial.println("Toggling");
      Serial.println("going right now because of the light");
      specialLightEndTime = 0; 
      
      toggleLight();
    }
    irPressCount = 0;
  }

  // A clap is detected if the MIC signal is HIGH.
  if (digitalRead(MIC_SIGNAL) == LOW) {
    // register a new clap if at least 100ms passed since the last one.
    if (now - lastClapTime > 100) {
      clapCount++;
      lastClapTime = now;
      Serial.print("Clap heard, clap count: ");
      Serial.println(clapCount);
    }
  }
  
  // If no new clap is detected for 500ms, check the clap count.
  if (clapCount > 0 && (now - lastClapTime > 500)) {
    if (clapCount == 2 && !inSpecialTimeStart) {
      toggleLight();
      Serial.println("2 claps heard");
      specialLightEndTime = 0; 
    }
    // Reset the clap counter after handling.
    clapCount = 0;
  }

  // Turn off automatically after 30 imnutes
  if (isLightOn && specialLightEndTime > 0 && now >= specialLightEndTime) {
    turnLightOff();
    specialLightEndTime = 0;
  }
}
