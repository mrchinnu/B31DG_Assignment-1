#define production
#ifdef production
#define debug 1
#else
#define debug 1000
#endif
const int ledPin = 16;
const int buttonPin = 22;
const int ledPin1 = 32;
const int buttonPin1 = 26;
const int Syn_ledPin=15;
const int alpha=1300;
const int beta=600;
const int cis=13;
const int delta=500;
const int t_s=50;
const int formula_50=50;

int buttonState, buttonState1;
int lastButtonState = HIGH, lastButtonState1 = HIGH;
volatile bool mode = false;      
volatile bool mode1 = false; 
int al;
int be;
int ci;
int de;
int x;
volatile unsigned long lastEnableInterruptTime = 0;
volatile unsigned long lastSelectInterruptTime = 0;
const unsigned long debounceDelayInterrupt = 250;

void IRAM_ATTR interupt_Enable() {
  unsigned long now = millis();
  if (now - lastEnableInterruptTime >= debounceDelayInterrupt) {
    mode = !mode;
    lastEnableInterruptTime = now;
  }
}

void IRAM_ATTR interupt_Select() {
  unsigned long now = millis();
  if (now - lastSelectInterruptTime >= debounceDelayInterrupt) {
    mode1 = !mode1;
    lastSelectInterruptTime = now;
  }
}

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(ledPin1, OUTPUT);
    pinMode(Syn_ledPin,OUTPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPin), interupt_Enable, FALLING);
    attachInterrupt(digitalPinToInterrupt(buttonPin1), interupt_Select, FALLING);
    Serial.begin(19200);
    Serial.println("Started");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState1 = digitalRead(buttonPin1);
  al=alpha;
  be=beta;
  de=delta;
  if(mode){
  ci=cis-3;
  }
  else{
  ci=cis;
  }
  digitalWrite(ledPin, mode);
  x=0;
  if(mode1){
    digitalWrite(Syn_ledPin,HIGH);
    delayMicroseconds(t_s*debug);
    digitalWrite(Syn_ledPin,LOW);
    while(x<ci){
      digitalWrite(ledPin1,HIGH);
      delayMicroseconds(al*debug);
      digitalWrite(ledPin1,LOW);
      delayMicroseconds(be*debug);
      al=alpha+(x+1)*formula_50;
      if(x==ci-1){
        delayMicroseconds(de*debug);
      }
      x++;
    }
  }
}
