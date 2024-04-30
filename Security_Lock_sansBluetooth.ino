#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
//#include <TimerOne.h>

//Keypad global code ___________ //
const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = {9,7,6,4};  
byte colPins[COLS] = {8,10,5};   

char passCode[4];
unsigned int pos = 0;

const char defaultCode[4] = "1234";
char mySetCode[4] = "----";
bool isCorrect = false;

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// OLED global code ___________ //
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM bluetooth_icon[] = {0x08,0x14,0x12,0x93,0x52,0x3c,0x3e,0x5f,0x91,0x13,0x14,0x18};
static const unsigned char PROGMEM bluetoothON_icon[] = {0x0f,0x80,0x3d,0xe0,0x7a,0xf0,0x7b,0x70,0xdb,0xb8,0xeb,0x78,0xf8,0xf8,0xf3,0x78,0xfb,0xb8,0x5b,0xb0,0x7b,0x70,0x3a,0xe0,0x0d,0x80};

volatile bool blink = false;
bool isConnected = false;
volatile int timerCount = 0;
const int waitTime = 7; //in seconds

// Servo global code ___________ //
#define servoPin 11
Servo myServo;
const int maxAngle = 95;
const int minAngle = 0;

// Timer global code
const int period = 500;
long holdTime = 0;

// Other global code
#define checkPin 3
volatile bool isOpen = true;
char lastKey = NULL;
unsigned long lastKeyPress = 0;
unsigned long lastOpenTime = 0;
unsigned long lastFailTime = 0;
unsigned long interval = 1.5 *1000;

void setup() {
  Serial.begin(9600);

  // OLED Setup --------- //
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setRotation(2);

  // Keypad Setup --------- //
  keypad.setHoldTime(2000);
  while (mySetCode[0] == '-' || mySetCode[0] == '-' || mySetCode[0] == '-' || mySetCode[0] == '-')
    for (int i = 0; i < 4; i++) {
      mySetCode[i] = EEPROM.read(i);
      Serial.print(mySetCode[i]);
    }

  // Servo Setup --------- //
  myServo.attach(servoPin);
  myServo.write(maxAngle);

  pinMode(checkPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(checkPin), ISR_checkPass, FALLING);
}

void loop() {
  char key = keypad.getKey();
  KeyState state = keypad.getState();
  if (key) keyPressHandler(key);
  if(lastKey == '0' && state == HOLD) 
    setNewPass();
  if(Serial.available() != 0) isConnected = true;

  if(isOpen)
      myServo.write(maxAngle);                      
  else
      myServo.write(minAngle);
  
  timing();

  if(millis() < 1000)
    drawStartScreen();
  else
    drawMainScreen();


  for (int i = 0; i < 4; i++) {
      Serial.print(mySetCode[i]);
  }
  Serial.print(" --- ");
  for (int i = 0; i < pos; i++) Serial.print(passCode[i]);
  Serial.print(" --- ");
  Serial.print(isOpen);
  Serial.println();
}

// MY Interrupt Service Routines
void ISR_checkPass() {
  isCorrect = true;
  for (int i = 0; i < 4; i++)
    if (passCode[i] != mySetCode[i])
      isCorrect = false;
  if(isCorrect) {isOpen=true; return;}
  
  for (int i = 0; i < 4; i++)
    if (passCode[i] != defaultCode[i]) {
      isCorrect = false;
      lastFailTime = millis();
      return;
    }
  isCorrect = true;
  isOpen=true;
}

void lockTimer(){
  blink = !blink;

  if(timerCount >= waitTime*2){ // After a number of seconds always lock door
    isOpen = false;
    timerCount = 0;
  }
  if(isOpen) timerCount++;
}

void keyPressHandler(char key) {
  if (key == '#') {
    isCorrect = false;
    pos = 0;
    for (int i = 0; i < sizeof(passCode); i++) passCode[i] = NULL;
  } else if (key == '*') {
    isCorrect = false;
    if (pos) pos--;
    passCode[pos] = NULL;
  } else if (pos < 4) {
    lastKey = key;
    lastKeyPress = millis();
    for(int i = 0; i <= pos; i++){

    }
    passCode[pos++] = key;
  }
}

void timing(){
  if(!isCorrect) 
    lastOpenTime = millis();
  else if(millis()-lastOpenTime > interval) {
    isCorrect = false;
    pos = 0;
    for (int i = 0; i < sizeof(passCode); i++) passCode[i] = NULL;
  }
  if(millis() - holdTime > period){
    lockTimer();
    holdTime = millis();
  }
}

void setNewPass(){
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(8, 18);
  display.print("New Password...");
  display.setCursor(8, 7);
  display.print("Setting");
  display.display();
  Serial.println("passed---");
  Serial.flush();
  while(Serial.available() == 0);
  Serial.readStringUntil('|');
  Serial.flush();
  String newPass = Serial.readStringUntil('|');
  if(newPass.length() == 4)
    for(int i = 0; i < 4; i++){
      byte num = (byte)newPass[i];
      if(num > 47 && num < 72){
        EEPROM.update(i, newPass[i]);
        mySetCode[i] = newPass[i];
      }
      delay(100);
    }
}

void drawMainScreen(){
  display.clearDisplay();
  if(blink && isConnected) 
    display.drawBitmap(113, 1, bluetoothON_icon, 13, 13, 1);
  else
    display.drawBitmap(115, 2, bluetooth_icon, 8, 12, 1);
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(100, 25);
  display.print("#=DEL");
  display.setCursor(100, 17);
  display.print("*=<-");

  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.setCursor(0, 0);
  display.setTextWrap(false);
  if(isCorrect){
    display.print("OPEN");
  }
  else if(millis() - lastFailTime < interval && lastOpenTime > 2000){
    display.print("FAIL");
  }
  else
    if(!isCorrect)
      for (int i = 1; i <= 4; i++)
        if (i == pos && millis() - lastKeyPress < 500)
          display.print(lastKey);
        else if(i <= pos)
          display.print('*');
        else
          display.print(' ');

  display.display();
}

void drawStartScreen(){
  display.clearDisplay();
  static const unsigned char PROGMEM image_hand_peace_sign_bits[] = {0x10,0x40,0x28,0xa0,0x28,0xa0,0x28,0xa0,0x25,0x20,0x15,0x40,0x15,0x40,0x32,0x40,0x53,0x80,0x48,0x40,0xae,0x40,0x99,0x20,0x52,0x20,0x64,0x40,0x44,0x40,0x20,0x80};
  static const unsigned char PROGMEM image_hand_high_five_bits[] = {0x00,0xc0,0x00,0x19,0x4c,0x00,0x15,0x54,0x00,0x15,0x54,0x00,0x15,0x54,0x00,0x13,0x69,0x80,0x0b,0x6a,0x80,0x09,0x4d,0x00,0x6c,0x19,0x00,0x95,0xfa,0x00,0xcc,0x04,0x00,0x22,0x6c,0x00,0x31,0x84,0x00,0x18,0x88,0x00,0x0c,0x88,0x00,0x06,0x10,0x00};
  display.drawBitmap(114, 7, image_hand_peace_sign_bits, 11, 16, 1);
  display.drawBitmap(3, 7, image_hand_high_five_bits, 17, 16, 1);
  display.setTextColor(1);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(26, 9);
  display.print("Welcome");
  display.display();
}