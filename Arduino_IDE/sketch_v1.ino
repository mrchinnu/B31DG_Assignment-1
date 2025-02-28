#define production
#ifdef production
#define debug 1
#else
#define debug 1000
#endif
const int Alt_Mode_LED = 16;
const int Alt_Mode_Btn = 22;
const int Data_Wave_LED = 32;
const int Data_Wave_Btn = 26;
const int Syn_Wave_LED=15;
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
volatile unsigned long last_Enable_Interrupt_Time = 0;
volatile unsigned long last_Select_Interrupt_Time = 0;
const unsigned long debounce_Delay_Interrupt = 250;

void IRAM_ATTR interupt_Enable() {
  unsigned long now = millis();
  if (now - last_Enable_Interrupt_Time >= debounce_Delay_Interrupt) {
    mode = !mode;
    last_Enable_Interrupt_Time = now;
  }
}

void IRAM_ATTR interupt_Select() {
  unsigned long now = millis();
  if (now - last_Select_Interrupt_Time >= debounce_Delay_Interrupt) {
    mode1 = !mode1;
    last_Select_Interrupt_Time = now;
  }
}

void setup() {
    pinMode(Alt_Mode_Btn, INPUT_PULLUP);
    pinMode(Alt_Mode_LED, OUTPUT);
    pinMode(Data_Wave_Btn, INPUT_PULLUP);
    pinMode(Data_Wave_LED, OUTPUT);
    pinMode(Syn_Wave_LED,OUTPUT);
    attachInterrupt(digitalPinToInterrupt(Alt_Mode_Btn), interupt_Enable, FALLING);
    attachInterrupt(digitalPinToInterrupt(Data_Wave_Btn), interupt_Select, FALLING);
    Serial.begin(19200);
    Serial.println("Started");
}

void loop() {
  buttonState = digitalRead(Alt_Mode_Btn);
  buttonState1 = digitalRead(Data_Wave_Btn);
  al=alpha;
  be=beta;
  de=delta;
  if(mode){
  ci=cis-3;
  }
  else{
  ci=cis;
  }
  digitalWrite(Alt_Mode_LED, mode);
  x=0;
  if(mode1){
    digitalWrite(Syn_Wave_LED,HIGH);
    delayMicroseconds(t_s*debug);
    digitalWrite(Syn_Wave_LED,LOW);
    while(x<ci){
      digitalWrite(Data_Wave_LED,HIGH);
      delayMicroseconds(al*debug);
      digitalWrite(Data_Wave_LED,LOW);
      delayMicroseconds(be*debug);
      al=alpha+(x+1)*formula_50;
      if(x==ci-1){
        delayMicroseconds(de*debug);
      }
      x++;
    }
  }
}
