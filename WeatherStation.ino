#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <IRremote.hpp>

// If using software SPI, define CLK and MOSI
#define OLED_CLK 8
#define OLED_MOSI 11
#define OLED_CS 10
#define OLED_RESET 9
#define OLED_DC 7

// this is software SPI, slower but any pins
Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Screen Modes
#define SIMP_COMBINED 0
#define VISI_COMBINED 1
#define TEMP_GAUGE 2
#define HUMI_GAUGE 3
#define PRESS_GAUGE 4
#define ALTI_GAUGE 5
#define SLIDE_SHOW 7
int currentScreen = -1;
bool isSlideShow = false;
long startTime = 0;
int interval = 5;

#define IR_PIN 3
// BMP sensor global values
Adafruit_BME280 bme;
#define SEALEVELPRESS (1013.25)

//Sensor Values
float temperature = 0.0;
float pressure = 0.0;
float humidity = 0.0;
float altitude = 0.0;

void setup() {
  Serial1.begin(9600);
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  // BMP sensor setup
  unsigned status = bme.begin(0x76);  

  // Display Setup
  display.begin();
  display.clearDisplay();  // clears the screen and buffer
  display.invertDisplay(1);
  drawSelectedScreen();
  display.display();
  delay(5000);
  display.invertDisplay(0);
}


void loop() {
  if (isSlideShow && millis()-startTime > interval*1000){
    if (currentScreen < 5) currentScreen++;
    else currentScreen = 0;
    startTime = millis();
  }
  else if(!isSlideShow)
    startTime = millis();
  readSensorReadings();
  printSensVals();
  display.clearDisplay();
  checkRemoteCommand();
  drawSelectedScreen();
  display.display();
}

void readSensorReadings(){
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  humidity = bme.readHumidity();
  altitude = bme.readAltitude(SEALEVELPRESS);
}

void printSensVals(){
  Serial1.print(temperature);
  Serial1.print("\t");
  Serial1.print(pressure);
  Serial1.print("\t");
  Serial1.print(humidity);
  Serial1.print("\t");
  Serial1.println(altitude);
}

void checkRemoteCommand() {
    if (IrReceiver.decode()) {
        IrReceiver.resume();
        if(!IrReceiver.decodedIRData.flags){
          isSlideShow = false;
          switch (IrReceiver.decodedIRData.command){
            case 0x09:  case 0x45:
              if(currentScreen < 5)currentScreen++;
              break;
            case 0x07:  case 0x46:
              if(currentScreen > 0)currentScreen--;
              break;
            case 0x16:  case 0x10:
              currentScreen = 0;
              break;
            case 0xC:  case 0x11:
              currentScreen = 1;
              break;
            case 0x18:  case 0x12:
              currentScreen = 2;
              break;
            case 0x5E:  case 0x13:
              currentScreen = 3;
              break;
            case 0x8:  case 0x14:
              currentScreen = 4;
              break;
            case 0x1C:  case 0x15:
              currentScreen = 5;
              break;
            case 0x40:  case 0x44:
              isSlideShow = true;
              break;         
          }
        }
    }
}

void drawSelectedScreen(){
  switch(currentScreen){
    case 0:
      drawSimpleCombinedScreen();
      break;
    case 1:
      drawVisualCombinedScreen();
      break;
    case 2: 
      drawTemperatureScreen();
      break;
    case 3:
      drawHumidityScreen();
      break;
    case 4:
      drawPressureScreen();
      break;
    case 5:
      drawAltitudeScreen();
      break;
    case -1:
      drawWelcomeScreen();
      currentScreen = 0;
    default:
      isSlideShow = true;
      break;
  }
}

String getFormatedVal(float val, int numSig, int numDec){
  String valStr = String(val,numDec);
  int length = String(round(floor(val))).length();
  String dec = String(val - floor(val), numDec);
  dec.remove(0,1);
  if(numSig-length > 0 )
    return "0" + getFormatedVal(val, numSig-1, numDec);
  else
    return String(int(floor(val)))+dec;
}

int getTextWidth(String text){
  return text.length()* 10 + text.length()*2 - 2;
}

void drawSlideShow(){
  
}

void drawVisualCombinedScreen() {
  static const unsigned char PROGMEM image_paint_0_bits[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x0a, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x70, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x09, 0x0c, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0xc0, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x08, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x10, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x10, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0xc0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x20, 0xa9, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x70, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x5d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7d, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x18, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x84, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x48, 0x1e, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xbe, 0x6d, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x9d, 0x92, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x89, 0x21, 0x20, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xd2, 0x21, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xf3, 0xff, 0xf0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x74, 0x40, 0x88, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0xf4, 0x40, 0x88, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x04, 0x40, 0x88, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x04, 0x44, 0x40, 0x88, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x03, 0x63, 0xff, 0xf0, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x01, 0xf2, 0x21, 0x10, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x61, 0x21, 0x20, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x41, 0x92, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
  display.drawBitmap(0, 0, image_paint_0_bits, 128, 64, 1);
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(85, 27);
  display.print(getFormatedVal(humidity, 2,1) + "%");
  display.setCursor(37, 57);
  display.print(getFormatedVal(pressure, 4,2) + " hPa");
  display.setCursor(12, 27);
  display.print(getFormatedVal(temperature, 2,1));
  display.print((char)247);  // degree symbol
  display.print("C");
}

void drawSimpleCombinedScreen() {
  display.setTextColor(1);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(23, 0);
  display.print("Weather");
  display.setTextSize(1);
  display.setCursor(18, 33);
  display.print("Humidity:");
  display.setCursor(71, 33);
  display.print(String(humidity,2) + "%");
  display.setCursor(18, 57);
  display.print("Altitude:");
  display.setCursor(70, 57);
  display.print(String(altitude,2) + "m");
  display.setCursor(18, 45);
  display.print("Pressure:");
  display.setCursor(70, 45);
  display.print(String(pressure,2) + "hPa");
  display.setCursor(0, 21);
  display.print("Temperature:");
  display.setCursor(71, 21);
  display.print(String(temperature,2));
  display.print((char)247);  // degree symbol
  display.print("C");
}

void drawTemperatureScreen() {
  static const unsigned char PROGMEM image_paint_0_bits[] = { 0x00, 0xff, 0x80, 0x00, 0x01, 0x00, 0x40, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x30, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02, 0x00, 0x20, 0x00, 0x06, 0x00, 0x30, 0x00, 0x08, 0x3e, 0x08, 0x00, 0x10, 0xff, 0x84, 0x00, 0x23, 0xff, 0xe2, 0x00, 0x27, 0xff, 0xf2, 0x00, 0x44, 0x7f, 0xf1, 0x00, 0x48, 0x3f, 0xf9, 0x00, 0x88, 0x3f, 0xf8, 0x80, 0x98, 0x3f, 0xfc, 0x80, 0x9c, 0x7f, 0xfc, 0x80, 0x9f, 0xff, 0xfc, 0x80, 0x9f, 0xff, 0xfc, 0x80, 0x9f, 0xff, 0xfc, 0x80, 0x8f, 0xff, 0xf8, 0x80, 0x4f, 0xff, 0xf9, 0x00, 0x47, 0xff, 0xf1, 0x00, 0x27, 0xff, 0xf2, 0x00, 0x23, 0xff, 0xe2, 0x00, 0x10, 0xff, 0x84, 0x00, 0x08, 0x3e, 0x08, 0x00, 0x06, 0x00, 0x30, 0x00, 0x01, 0x80, 0xc0, 0x00, 0x00, 0x7f, 0x00, 0x00 };
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(58, 22);
  display.print("Temperature:");
  display.fillRect(14, 4, 7, 40, 1);
  display.setCursor(26, 2);
  display.print("50");  // Highest temperatire ever recorded was 57 degrees
  display.setTextSize(2);
  String tempStr = String(temperature,2) + char(247) + "C";
  int pos = 128-getTextWidth(tempStr) -2;
  display.setCursor(pos, 37);
  display.drawLine(pos, 32, 128, 32, WHITE);
  display.print(tempStr);
  display.drawBitmap(5, 0, image_paint_0_bits, 25, 64, 1);
  if(temperature < 0)
    display.fillRect(12, 4, 11, 40, 0);
  else if(temperature <= 50){
      int pos = round(temperature / 50.0 * 37);
      display.fillRect(12, 4, 11, 37-pos, 0);
  }
}

void drawPressureScreen() {
  // Static pixels
  static const unsigned char PROGMEM image_paint_0_bits[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  display.drawBitmap(8, 4, image_paint_0_bits, 111, 58, 1);
  display.drawCircle(63, 59, 59, 1);
  display.drawCircle(63, 59, 57, 1);
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(97, 57);
  display.print("108");
  display.setCursor(13, 57);
  display.print("80");
  display.setCursor(58, 8);
  display.print("94");
  display.setCursor(26, 25);
  display.print("87");
  display.setCursor(90, 25);
  display.print("101");
  String presStr = String(pressure,2) + "hPa";
  display.setCursor(35, 38);
  if (pressure < 1000) display.setCursor(41, 38);
  display.print(presStr);
  display.drawPixel(63, 61, 1);

  // Animated pixels
  float angle = (pressure/10 - 80.0) / 28.0 * radians(180);
  int x0 = 63 - round(44 * cos(angle));
  int y0 = 62 - round(44 * sin(angle));

  float offset = radians(45);
  int x1 = 63 - round(5 * cos(angle - offset));
  int y1 = 61 - round(5 * sin(angle - offset));
  int x2 = 63 - round(5 * cos(angle + offset));
  int y2 = 61 - round(5 * sin(angle + offset));

  display.fillTriangle(x0, y0, x1, y1, x2, y2, 0);
  display.drawLine(x1, y1, x0, y0, 1);
  display.drawLine(x2, y2, x0, y0, 1);
  display.drawCircle(63, 61, 4, 1);
}


void drawHumidityScreen() {
  static const unsigned char PROGMEM image_paint_0_bits[] = { 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0c, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x02, 0x19, 0x80, 0x00, 0x00, 0x04, 0x00, 0x03, 0x30, 0xc0, 0x00, 0x00, 0x0c, 0x00, 0x01, 0x60, 0x40, 0x00, 0x00, 0x18, 0x00, 0x01, 0xc0, 0x60, 0x00, 0x00, 0x10, 0x00, 0x18, 0xc0, 0x20, 0x00, 0x00, 0x30, 0x00, 0x18, 0x40, 0x30, 0x00, 0x00, 0x20, 0x00, 0x0c, 0x60, 0x10, 0x00, 0x00, 0x60, 0x00, 0x0e, 0x20, 0x18, 0x00, 0x00, 0x40, 0x00, 0x06, 0x30, 0x0c, 0x00, 0x00, 0xc0, 0x00, 0x03, 0x18, 0x04, 0x00, 0x01, 0x80, 0x00, 0x03, 0x88, 0x06, 0x00, 0x01, 0x00, 0x00, 0x01, 0x8c, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 0x06, 0x00, 0x00, 0x00, 0x62, 0x00, 0x80, 0x0c, 0x00, 0x00, 0x00, 0x63, 0x00, 0x80, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x18, 0x00, 0x00, 0x00, 0x01, 0x80, 0x40, 0x18, 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x30, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x40, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x10, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x10, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x10, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x11, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x00 };
  static const unsigned char PROGMEM image_paint_1_bits[] = { 0x00, 0xf8, 0x07, 0xe0, 0x00, 0x03, 0xff, 0x1f, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00 };
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(77, 20);
  display.print("Humidity:");
  display.setTextSize(2);
  display.setCursor(58, 37);
  display.print(String(humidity,2)+ "%");
  display.drawLine(58, 32, 128, 32, WHITE);
  display.fillTriangle(22, 5, 5, 35, 39, 36, 1);
  display.drawBitmap(5, 32, image_paint_1_bits, 35, 27, 1);
  display.setCursor(5, 34);
  display.setTextColor(0);
  int perc_r = round(humidity);
  display.print(perc_r);
  display.print("%");

  int height = round(humidity / 100.0 * 54);
  display.fillRect(5, 5, 35, 54 - height, 0);
  display.drawBitmap(0, 0, image_paint_0_bits, 52, 64, 1);
}

void drawAltitudeScreen() {
  static const unsigned char PROGMEM image__1090863_bits[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0e, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0b, 0x80, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x80, 0xee, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0xc1, 0xee, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0xc1, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03, 0xc3, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x03, 0xe7, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0xef, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xff, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0xfe, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x3c, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x3c, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x3e, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x18, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x08, 0x00, 0x3c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x00, 0x7c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x61, 0x80, 0x00, 0x0c, 0x00, 0x7e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x04, 0x00, 0xfe, 0x36, 0x00, 0x00, 0x00, 0x00, 0x71, 0x00, 0x00, 0x1c, 0x00, 0xff, 0x72, 0x00, 0x00, 0x00, 0x00, 0xda, 0x00, 0x00, 0x1c, 0x08, 0xfd, 0xe3, 0x00, 0x00, 0x00, 0x00, 0xce, 0x00, 0x00, 0x1c, 0x10, 0xfd, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x00, 0x10, 0x3c, 0x31, 0xfc, 0xc1, 0x04, 0x00, 0x00, 0x01, 0x84, 0x00, 0x10, 0x3e, 0x61, 0xfe, 0x91, 0x9c, 0x00, 0x00, 0x01, 0x80, 0x00, 0x90, 0x3e, 0xe3, 0xfe, 0x14, 0xfe, 0x00, 0x00, 0x01, 0x00, 0x00, 0x98, 0x7f, 0xc3, 0xfe, 0x3c, 0xfe, 0x00, 0x00, 0x03, 0x00, 0x00, 0xd8, 0x7f, 0xc7, 0xde, 0x7c, 0x67, 0x00, 0x00, 0x03, 0x00, 0x00, 0xd8, 0x7f, 0x87, 0xcf, 0x7c, 0x07, 0x00, 0x00, 0x02, 0x00, 0x00, 0xfc, 0x7f, 0x8f, 0xcf, 0xfe, 0x03, 0x80, 0x00, 0x02, 0x00, 0x00, 0xfc, 0x7f, 0x0f, 0x8f, 0xfe, 0x01, 0x80, 0x00, 0x06, 0x04, 0x10, 0xfe, 0x7f, 0x1f, 0x8f, 0xfe, 0x01, 0xc0, 0x00, 0x04, 0x04, 0x10, 0xff, 0x7f, 0x1f, 0x07, 0xfe, 0xc0, 0xe0, 0x00, 0x04, 0x0e, 0x18, 0xff, 0x7f, 0x3f, 0x07, 0xff, 0xe0, 0xe0, 0x00, 0x0c, 0x0f, 0x3c, 0xff, 0xff, 0x3e, 0x07, 0xff, 0xf0, 0x70, 0x00, 0x08, 0x0f, 0x7f, 0xff, 0xff, 0x7e, 0x03, 0xff, 0xf8, 0x30, 0x00, 0x08, 0x1f, 0x7f, 0xff, 0xff, 0x7e, 0x03, 0xff, 0xf8, 0x38, 0x00, 0x08, 0x1f, 0xff, 0xfc, 0xff, 0xfe, 0x01, 0xff, 0xfc, 0x18, 0x00, 0x10, 0x7f, 0xff, 0xfc, 0x3f, 0xfe, 0x01, 0xff, 0xfe, 0x9c, 0x00, 0x11, 0xff, 0xff, 0x88, 0x3f, 0xfe, 0x30, 0xff, 0xff, 0xcc, 0x00, 0x11, 0xff, 0xff, 0x00, 0x3f, 0xfe, 0x3c, 0xff, 0xff, 0xe6, 0x00, 0x21, 0xff, 0xff, 0x00, 0x3f, 0xfe, 0x3e, 0x7f, 0xff, 0xf6, 0x00, 0x23, 0xff, 0xfe, 0x00, 0xff, 0xfe, 0x3f, 0x3f, 0xff, 0xfb, 0x00, 0x23, 0xff, 0xfe, 0x31, 0xff, 0xff, 0x7f, 0xbf, 0xff, 0xff, 0x00, 0x47, 0xff, 0xfc, 0x77, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x4f, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x9f, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  static const unsigned char PROGMEM image_toilets_gentlemen_bits[] = { 0x10, 0x38, 0x38, 0x10, 0x6c, 0xfe, 0xee, 0xfe, 0xee, 0xfe, 0x7c, 0x6c, 0x6c, 0x28, 0x28, 0x6c };
  display.drawBitmap(-24, 2, image__1090863_bits, 83, 64, 1);
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(77, 20);
  display.print("Altitude:");
  display.setTextSize(2);
  display.setCursor(82, 37);
  String altStr = String(altitude,1) + "m"; // Aproximate altitude
  int pos = 128-getTextWidth(altStr);
  display.setCursor(pos, 37);
  display.drawLine(pos, 32, 128, 32, WHITE);
  display.print(altStr);
  int y = 48 - round(altitude / 100.0 * 48);
  int x = (altitude <= 53) ? (29 * y + 1440) / 48 : y + 21;
  display.drawBitmap(x, y, image_toilets_gentlemen_bits, 7, 16, 1);
}

void drawWelcomeScreen() {
  static const unsigned char PROGMEM image_tre_bits[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x30, 0x00, 0x00, 0x00, 0x00, 0x07, 0x18, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe4, 0x18, 0x00, 0x00, 0x00, 0x00, 0x1d, 0xf0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x98, 0x00, 0x00, 0x00, 0x38, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xf0, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x36, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x13, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x8e, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1a, 0x41, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x06, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x32, 0x61, 0x80, 0x00, 0x00, 0x86, 0x03, 0xff, 0xfc, 0x03, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xf2, 0x22, 0x30, 0x80, 0x00, 0x0f, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x10, 0x80, 0x00, 0x00, 0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x23, 0x10, 0x80, 0x00, 0x00, 0x69, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x21, 0x10, 0xf8, 0x00, 0x00, 0x2c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x86, 0x21, 0x10, 0xcc, 0x00, 0x07, 0xb7, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x21, 0x13, 0x87, 0xc0, 0x00, 0x11, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x21, 0x1e, 0x00, 0x70, 0x78, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x01, 0x13, 0x30, 0x00, 0x18, 0x00, 0x40, 0x21, 0xab, 0x5a, 0xb5, 0x63, 0xf0, 0x00, 0x00, 0x00, 0x01, 0x12, 0x60, 0x00, 0x0c, 0x0f, 0x7f, 0xc1, 0xab, 0x5a, 0xb5, 0x61, 0xfc, 0x00, 0x00, 0x0f, 0xf9, 0x12, 0xff, 0xff, 0xff, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x92, 0x22, 0x00, 0x00, 0x00, 0x73, 0xc0, 0x01, 0xff, 0xf8, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x92, 0x22, 0x00, 0x00, 0x3c, 0xc0, 0x60, 0x01, 0x01, 0xfc, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x26, 0x00, 0x00, 0x01, 0x80, 0x3f, 0xff, 0xff, 0x07, 0xff, 0xf8, 0x00, 0x07, 0xfc, 0x00, 0x52, 0x4c, 0x00, 0x00, 0x03, 0x00, 0x18, 0x00, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7a, 0x58, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x50, 0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x1f, 0xbd, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x1d, 0xa0, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x07, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x06, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0x80, 0x0f, 0xc0, 0x00, 0x00, 0x02, 0x00, 0x00, 0xc0, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x30, 0x00, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x40, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x40, 0x07, 0x80, 0x00, 0x00, 0xff, 0xfe, 0x03, 0xff, 0xe7, 0x21, 0xc0, 0x00, 0x00, 0x60, 0x00, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xe0, 0x60, 0x00, 0x00, 0x40, 0x00, 0x40, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x30, 0x00, 0x00, 0x40, 0x00, 0x78, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x00, 0x40, 0x00, 0x08, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x0c, 0x00, 0x00, 0x60, 0x00, 0x0e, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x04, 0x00, 0x00, 0x30, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0x06, 0x00, 0x3f, 0xff, 0xe0, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  display.drawBitmap(0, 9, image_tre_bits, 128, 59, 1);
  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(7, 3);
  display.print("Weather Station");
}
