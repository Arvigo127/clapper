#define MIC_IN 48
#define RELAY_OUT 50
#define RELAY_POWER 53

bool lightOn = false;
long firstSoundTime = 00L;
bool firstClapHeard = false;

bool lastSound = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(MIC_IN, INPUT);
  pinMode(RELAY_OUT, OUTPUT);
  pinMode(RELAY_POWER, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(RELAY_POWER, HIGH);
  if(digitalRead(MIC_IN) && lastSound) {
    delay(25);
    if(!firstClapHeard) {
      firstClapHeard = true;
      firstSoundTime = millis();
      delay(50);
            
    } else if((millis() - firstSoundTime) <= 800){
        lightOn = !lightOn;
        digitalWrite(RELAY_OUT, lightOn);
        firstClapHeard = false;
        firstSoundTime = millis();
        delay(300);
    } else {
      firstClapHeard = false;
    }
  }

  if((millis() - firstSoundTime) > 1000) {
    firstClapHeard = false;
    firstSoundTime = millis();
  }  

  lastSound = !digitalRead(MIC_IN);
  
}
