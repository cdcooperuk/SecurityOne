#include <LiquidCrystal.h>

//range
const int  pinTrig = 9;
const int  pinEcho = 10;

//pir
const int pinPir =11;

//buzzer
const int pinBuzzer =13;

boolean pirDetected = false;
int rangeCm = 42;
int speedSoundMicrosPerCm = 29;

int cycle = 0;

//button
const int pinBacklightButton=12;

//lcd
const int  pinBacklight = 14;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2,8,3,4,5,6,7);
byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};
boolean backlightOn=false;

const uint8_t cc=0;
void setup() {
  initLcd();
  initPir();
  initRange();
  initBuzzer();
  initBacklightButton();
}

void initLcd(){
  lcd.createChar(cc, smiley);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Security One");
  lcd.write(cc);
  pinMode(pinBacklight, OUTPUT);
}
void initBacklightButton(){
  pinMode( pinBacklightButton , INPUT_PULLUP);
}
void initPir(){
  pinMode(pinPir, INPUT);
}

void  initRange(){
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
}

void initBuzzer(){
  pinMode( pinBuzzer ,OUTPUT);
  digitalWrite( pinBuzzer ,LOW);
}


void loop() {
  handleBacklightButton();

  handlePir();

  handleRange();

  updateStatus();

  delay(100);
}

/* return true if pir detected */
boolean handlePir(){
  pirDetected=digitalRead(pinPir);
}

boolean handleBacklightButton(){
  if (! digitalRead( pinBacklightButton )){
    backlightOn = ! backlightOn;
  }
  digitalWrite( pinBacklight , backlightOn?HIGH:LOW);
}

void handleRange(){
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite( pinTrig , HIGH);
  delayMicroseconds(10);
  digitalWrite( pinTrig , LOW);
  delayMicroseconds(2);

  int duration = pulseIn(pinEcho, HIGH, 50000);
  int curRangeCm = duration / speedSoundMicrosPerCm / 2; 
  if (curRangeCm<=500 && curRangeCm>0) {
    rangeCm = curRangeCm;
  }
}

void updateStatus(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(++cycle);
  lcd.print(" ");
  lcd.print("PIR=");
  lcd.print(pirDetected?"TRUE":"FALSE");
  lcd.setCursor(0, 1);
  lcd.print("range=");
  lcd.print(rangeCm);  
  lcd.print("cm  ");


  if (pirDetected) {
    int freq = map(rangeCm,500,0,270,3000);
    //long durMs = map(rangeCm,500,0,200,100); 
    long durMs = 100;
    tone( pinBuzzer ,freq, durMs);
  } 
  else {
    noTone( pinBuzzer );
  }

}

