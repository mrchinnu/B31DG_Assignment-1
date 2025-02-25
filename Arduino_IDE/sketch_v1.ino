
const int debug=1;
const int ledPin = 16;
const int buttonPin = 22;
const int ledPin1 = 32;
const int buttonPin1 = 26;
const int Syn_ledPin=15;
const int alpha=1300;
const int beta=600;
const int cis=13;
const int delta=50;
const int t_s=50;

int buttonState, buttonState1;
int lastButtonState = HIGH, lastButtonState1 = HIGH;
int mode = 0, mode1 = 0;
int al;
int be;
int ci;
int de;
int x;

unsigned long lastDebounceTime = millis(), lastDebounceTime1 = millis();
const unsigned long debounceDelay = 250; 



void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(ledPin1, OUTPUT);
    pinMode(Syn_ledPin,OUTPUT);
    Serial.begin(19200);
    Serial.println("Started");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState1 = digitalRead(buttonPin1);
  al=alpha;
  be=beta;
  de=delta;

  if ((millis() - lastDebounceTime) >= debounceDelay) {
      if (buttonState == LOW && lastButtonState == HIGH) {
          mode = !mode;
          lastDebounceTime = millis();
      }
  }

  if(mode){
  ci=cis-3;
  }
  else{
  ci=cis;
  }
  if ((millis() - lastDebounceTime1) >= debounceDelay) {
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
        mode1=!mode1;
        lastDebounceTime1 = millis();
    }
  }
  lastButtonState = buttonState;
  lastButtonState1 = buttonState1;

  digitalWrite(ledPin, mode);
  x=0;
  if(mode1){
    digitalWrite(Syn_ledPin,HIGH);
    delayMicroseconds(t_s*debug);
    digitalWrite(Syn_ledPin,LOW);
    while(x<ci){
      digitalWrite(ledPin1,HIGH);
      delayMicroseconds(al*debug);
      digitalWrite(ledPin1,LOW);``
      delayMicroseconds(be*debug);
      al=alpha+(x+1)*50;
      if(x==ci-1){
        delayMicroseconds(de*debug);
      }
      x++;
    }
  }
}
