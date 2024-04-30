#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <DueTimer.h>


// Clock 11, MISO 12, CS 10,13
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2_time(U8G2_R0, 11, 12, 10, U8X8_PIN_NONE, U8X8_PIN_NONE);  
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2_sense(U8G2_R2, 11, 12, 13, U8X8_PIN_NONE, U8X8_PIN_NONE);
#define restartPin 3

//TIme global code
uint8_t hour = 7;
uint8_t min =49;
uint8_t sec =11;

//Sensor global code
#define senseType DHT22
#define sensePin 2
DHT dht(sensePin, senseType);
float temperature = 0;
float humidity = 0;
bool senseMode = 1;

//Light Switch global code
#define servPin 9
#define ON_STATE 95
#define OFF_STATE 0
#define REST_STATE 45
int pulseWidth = 1500;
uint8_t lightState = 2;

void setup(void) {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(restartPin, INPUT_PULLUP);
  u8g2_time.begin();
  u8g2_time.setContrast(10*16);
  u8g2_sense.begin();
  u8g2_sense.setContrast(10*16);

  Timer8.attachInterrupt(ISR_incrementTime);
  Timer8.start(1000000);

  pinMode(servPin, OUTPUT);
  Timer7.attachInterrupt(servoTimerInterrupt);
  Timer7.setPeriod(25000);
  Timer7.start();

  Timer5.attachInterrupt(ISRservoReset);
  Timer5.setPeriod(1000000);
}

void loop(void) {
  drawSensorData();
  drawTimeMatrix();
  if(Serial.available() != 0)getBtInput();
  manageStates();
}









///............Working functions and ISRs...............///
void manageStates(){
  if(!digitalRead(restartPin))restartDisplay();
  if(lightState < 2){
    if(lightState)
      moveServo(ON_STATE);
    else if(!lightState)
      moveServo(OFF_STATE);
  }
  else
    moveServo(REST_STATE);

  // Serial.print(hour);
  // Serial.print(":");
  // Serial.print(min);
  // Serial.print(":");
  // Serial.print(sec);
  // Serial.print("\t");
  // Serial.println(lightState);
  // Printing data
  Serial.print(temperature);
  Serial.print("\t");
  Serial.println(humidity);
}
void ISRservoReset(){
  lightState = 2;
    Timer5.stop();
}

void drawSensorData(){
  u8g2_sense.clearBuffer();
  String sensStr;
  if(!senseMode){
    static const unsigned char deg[] U8X8_PROGMEM = {0x0e,0x09,0x09,0x06};
    u8g2_sense.drawXBM(21, 0, 4, 4, deg);
    u8g2_sense.drawStr(23+1, 8, " C");
    u8g2_sense.setFont(u8g2_font_timR08_tf);
    temperature = dht.readTemperature();
    sensStr = String(temperature, 1);
    u8g2_sense.drawStr(3, 8, sensStr.c_str());
  }
  else{
    static const unsigned char perc[] U8X8_PROGMEM = {0x47,0x25,0x13,0x08,0x64,0x52,0x71};
    u8g2_sense.drawXBM(25, 1, 7, 7, perc);
    u8g2_sense.setFont(u8g2_font_new3x9pixelfont_tf);
    humidity = dht.readHumidity();
    sensStr = String(humidity, 1);
    u8g2_sense.drawStr(11, 8, sensStr.c_str());
  }
  u8g2_sense.sendBuffer();
}

void drawTimeMatrix(){
  u8g2_time.clearBuffer();	// clear the internal memory
  u8g2_time.setFont(u8g2_font_nerhoe_tn);	// choose a suitable font
  //hour
  if(hour < 10)
    u8g2_time.drawStr(4,8, String(hour).c_str());
  else if(hour < 20)
    u8g2_time.drawStr(1,8, String(hour).c_str());
  else
    u8g2_time.drawStr(0,8, String(hour).c_str());
  //minutes
  u8g2_time.drawStr(12,8,fVal(min,2).c_str());
  //Seconds
  String secStr = fVal(sec,2);
  u8g2_time.drawStr(24,8, String(secStr[0]).c_str());
  u8g2_time.setFont(u8g2_font_3x5im_tr);
  u8g2_time.drawStr(29,5, String(secStr[1]).c_str());
  // Colons
  u8g2_time.drawStr(9,8,":");
  u8g2_time.drawStr(22,8,":");
  u8g2_time.sendBuffer();// transfer internal memory to the display
}

void ISR_incrementTime(){
  // Hours
  if(min == 59 && sec == 59){
    min = 0;
    sec = 0;
    if(hour == 23)
      hour = 0;
    else
      hour++;
  }
  else if(sec == 59){
    sec = 0;
    if(min == 59)
      min = 0;
    else
      min++;
  }
  else if(sec == 59)
    sec = 0;
  else
    sec++;
  senseMode = !senseMode;
}

String fVal(uint8_t val, uint8_t numSig){
  String valStr = String(val);
  if(numSig-valStr.length() > 0 )
    return "0" + fVal(val, numSig-1);
  else
    return valStr;
}

void moveServo(int pos){
    pulseWidth = map(pos, 0, 180, 500, 2500);
}

void servoTimerInterrupt() {
  digitalWrite(servPin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(servPin, LOW);
}

void getBtInput(){
  Serial.readStringUntil('|');
  String inp = Serial.readStringUntil('|');
  Serial.println(inp);
  if(inp.length() == 1){
    Timer5.start();
    byte num = inp.toInt();
    lightState = (num == 1 || num == 0)? num: lightState;
  }
  else if(inp.length() == 8){
    int tempNum = inp.substring(0,2).toInt();
    hour = (tempNum >= 0 && tempNum <= 23)? tempNum: hour;
    tempNum = inp.substring(3,5).toInt();
    min = (tempNum >= 0 && tempNum <= 59)? tempNum: min;
    tempNum = inp.substring(6).toInt();
    sec = (tempNum >= 0 && tempNum <= 59)? tempNum: sec;
  }else if(inp.equals("Restart")){
    restartDisplay();
  }
}

void restartDisplay(){
  Serial.println("Restarting displays...");
  u8g2_time.begin();
  u8g2_time.setContrast(10*16);
  u8g2_sense.begin();
  u8g2_sense.setContrast(10*16);
}