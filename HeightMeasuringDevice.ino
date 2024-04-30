#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimerOne.h>

// Oled global code
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM bluetooth_icon[] = {0x08,0x14,0x12,0x93,0x52,0x3c,0x3e,0x5f,0x91,0x13,0x14,0x18};
static const unsigned char PROGMEM bluetoothON_icon[] = {0x0f,0x80,0x3d,0xe0,0x7a,0xf0,0x7b,0x70,0xdb,0xb8,0xeb,0x78,0xf8,0xf8,0xf3,0x78,0xfb,0xb8,0x5b,0xb0,0x7b,0x70,0x3a,0xe0,0x0d,0x80};

// Sonar global code
const int trigPin = 9;
const int echoPin = 10;
const int numMeas = 100;
float height = 0;

// other global code
#define modePin 3
#define readPin 2
volatile bool modeState = false; // Mode of conversion ie. CM-->false or Feet and inches-->true
volatile bool isPressed = false;
int percent = 0;
volatile bool blink = false;

bool isConnected = false;

void setup() {
Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  drawWelcomeScreen();

  // Sonar Setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(modePin, INPUT_PULLUP);
  pinMode(readPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(modePin), ISR_MODE, FALLING);
  attachInterrupt(digitalPinToInterrupt(readPin), ISR_READ, FALLING);

  Timer1.initialize(500000);
  Timer1.attachInterrupt(ISR_TIMER);
  Timer1.start();

  delay(3000); // Pause for 2 seconds
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.read() != -1) isConnected = true;
  drawMainScreen(false);
  if(isPressed){
    height = calcHeight();
    isPressed = false;
  }
  Serial.print(modeState);
  Serial.print("\t");
  Serial.print(height);
  Serial.println();
}

float calcHeight(){
  float sumHeight = 0;
  for(float i = 0; i < numMeas; i++){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    int pingTravelTime = pulseIn(echoPin, HIGH);
    sumHeight += (pingTravelTime * 0.033145)/2;
  }
  float averageHeight = sumHeight/ numMeas;
  drawMainScreen(true);
  if (!modeState)
    return averageHeight;
  else {
    float totalInches = averageHeight / 2.54;
    // Calculate feet and remaining inches
    int feet = int(totalInches / 12);
    float inches = (totalInches - feet * 12)/100.0;
    return feet + inches;
  }
}

void ISR_MODE(){
  modeState = !modeState;
}
void ISR_READ(){
  isPressed = true;
}
void ISR_TIMER(){
  blink = !blink;
}


void drawMainScreen(bool isLoading){
  display.clearDisplay();
  display.drawLine(127, 21, 0, 21, 1);
  String units; 
  if(blink && isConnected) 
    display.drawBitmap(113, 3, bluetoothON_icon, 13, 13, 1);
  else
    display.drawBitmap(115, 4, bluetooth_icon, 8, 12, 1);

  if(!modeState){
    display.fillRect(5, 5, 27, 11, 1);
    display.drawRect(37, 5, 27, 11, 1);
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(13, 7);
    display.setTextWrap(false);
    display.print("CM");
    display.setTextColor(1);
    display.setTextSize(1);
    display.setCursor(39, 7);
    display.setTextWrap(false);
    display.print("f'i\"");
    units=" cm"; // String function
  }else{
    display.drawRect(5, 5, 27, 11, 1);
    display.fillRect(37, 5, 27, 11, 1);
    display.setTextColor(1);
    display.setTextSize(1);
    display.setCursor(13, 7);
    display.setTextWrap(false);
    display.print("CM");
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(39, 7);
    display.setTextWrap(false);
    display.print("f'i\"");
    units = " f'i\"";
  }
  const int rectWidth = 114;
  if (isLoading){
    for (int i = 0; i < numMeas; i++){
      display.clearDisplay();
      static const unsigned char PROGMEM image_hour_glass_75_bits[] = {0xff,0xe0,0x40,0x40,0x40,0x40,0x51,0x40,0x5f,0x40,0x2e,0x80,0x15,0x00,0x0a,0x00,0x0a,0x00,0x11,0x00,0x24,0x80,0x44,0x40,0x4e,0x40,0x5f,0x40,0x7f,0xc0,0xff,0xe0};
      static const unsigned char PROGMEM image_hour_glass_77_bits[] = {0x80,0x01,0xf8,0x1f,0xc4,0x21,0xe2,0x59,0xf1,0xb1,0xfc,0x71,0xf1,0xb1,0xe2,0x59,0xc4,0x21,0xf8,0x1f,0x80,0x01};
      display.setTextColor(1);
      display.setTextSize(1);
      display.setTextWrap(false);
      display.setCursor(27, 28);
      display.print("calculating...");
      display.fillRect(7, 43, int(rectWidth*(i/100.00)), 14, 1);
      display.drawRect(4, 40, 120, 20, 1);
      if(i % 4 == 0)
        display.drawBitmap(7, 23, image_hour_glass_75_bits, 11, 16, 1);
      else if (i % 2 == 0)
        display.drawBitmap(4, 25, image_hour_glass_77_bits, 16, 11, 1);
      display.display();
      delay(15);
    }
  }
  else{
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(2, 28);
    display.setTextWrap(false);
    display.print("Height:");

    display.setTextColor(1);
    display.setTextSize(2);
    display.setCursor(7, 43);
    display.setTextWrap(false);
    display.print(height);
    display.print(units);
  }
  display.display();
}

void drawWelcomeScreen(){
  display.clearDisplay();
  static const unsigned char PROGMEM image_music_fast_forward_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x00,0xc0,0x60,0x00,0xb0,0x58,0x00,0x8c,0x46,0x00,0x83,0x41,0x80,0x8c,0x46,0x00,0xb0,0x58,0x00,0xc0,0x60,0x00,0x80,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char PROGMEM image_car_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xf0,0x00,0x0c,0x88,0x00,0x18,0x84,0x00,0x7f,0xff,0xc0,0xf7,0xfd,0xa0,0xeb,0xfa,0xe0,0x55,0xf5,0x40,0x08,0x02,0x00};
  static const unsigned char PROGMEM image_cloud_bits[] = {0x00,0x00,0x00,0x07,0xc0,0x00,0x08,0x20,0x00,0x10,0x10,0x00,0x30,0x08,0x00,0x40,0x0e,0x00,0x80,0x01,0x00,0x80,0x00,0x80,0x40,0x00,0x80,0x3f,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char PROGMEM image_cloud_upload_2_bits[] = {0x00,0x00,0x00,0x07,0xc0,0x00,0x08,0x20,0x00,0x10,0x10,0x00,0x30,0x08,0x00,0x40,0x0e,0x00,0x80,0x01,0x00,0x80,0x00,0x80,0x41,0x00,0x80,0x33,0x9f,0x00,0x07,0xc0,0x00,0x01,0x00,0x00,0x03,0x00,0x00,0x02,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char PROGMEM image_earth_bits[] = {0x07,0xc0,0x1e,0x70,0x27,0xf8,0x61,0xe4,0x43,0xe4,0x87,0xca,0x9f,0xf6,0xdf,0x82,0xdf,0x82,0xe3,0xc2,0x61,0xf4,0x70,0xf4,0x31,0xf8,0x1b,0xf0,0x07,0xc0,0x00,0x00};
  static const unsigned char PROGMEM image_tree_bits[] = {0x05,0x40,0x2a,0xd4,0x57,0x26,0x31,0xb8,0x5d,0xea,0x87,0x95,0x6b,0x36,0xbb,0x68,0x4f,0xfd,0x37,0xc6,0x51,0xa9,0x29,0x82,0x01,0x80,0x01,0x80,0x03,0xc0,0x0f,0xf0};
  static const unsigned char PROGMEM image_toilets_gentlemen_bits[] = {0x10,0x38,0x38,0x10,0x6c,0xfe,0xee,0xfe,0xee,0xfe,0x7c,0x6c,0x6c,0x28,0x28,0x6c};
  static const unsigned char PROGMEM image_weather_umbrella_bits[] = {0x00,0x00,0x01,0x00,0x07,0xc0,0x1f,0xf0,0x3f,0xf8,0x7f,0xfc,0xff,0xfe,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00,0x00,0x00};
  static const unsigned char PROGMEM image_weather_cloud_snow_bits[] = {0x00,0x00,0x00,0x07,0xc0,0x00,0x08,0x20,0x00,0x10,0x10,0x00,0x30,0x08,0x00,0x40,0x0e,0x00,0x80,0x01,0x00,0x80,0x00,0x80,0x40,0x00,0x80,0x3f,0xff,0x00,0x00,0x00,0x00,0x09,0x4a,0x00,0x20,0x00,0x00,0x08,0x90,0x00,0x22,0x44,0x00,0x00,0x00,0x00};
  static const unsigned char PROGMEM image_weather_cloud_rain_bits[] = {0x00,0x00,0x00,0x07,0xc0,0x00,0x08,0x20,0x00,0x10,0x10,0x00,0x30,0x08,0x00,0x40,0x0e,0x00,0x80,0x01,0x00,0x80,0x00,0x80,0x40,0x00,0x80,0x3f,0xff,0x00,0x01,0x10,0x00,0x22,0x22,0x00,0x44,0x84,0x00,0x91,0x28,0x00,0x22,0x40,0x00,0x00,0x80,0x00};
  static const unsigned char PROGMEM image_plane_bits[] = {0x00,0x07,0x70,0x09,0x8e,0x11,0xc1,0xa2,0x30,0x44,0x0c,0x08,0x03,0x10,0x01,0x08,0x02,0xc8,0x05,0x44,0x7a,0x24,0x8c,0x24,0x64,0x12,0x14,0x12,0x14,0x0a,0x08,0x0c};
  static const unsigned char PROGMEM image_weather_cloud_sunny_bits[] = {0x00,0x20,0x00,0x02,0x02,0x00,0x00,0x70,0x00,0x01,0x8c,0x00,0x09,0x04,0x80,0x02,0x02,0x00,0x02,0x02,0x00,0x07,0x82,0x00,0x08,0x44,0x80,0x10,0x2c,0x00,0x30,0x30,0x00,0x60,0x1e,0x00,0x80,0x03,0x00,0x80,0x01,0x00,0x80,0x01,0x00,0x7f,0xfe,0x00};
  display.setTextColor(1);
  display.setTextSize(3);
  display.setTextWrap(false);
  display.setCursor(3, 18);
  display.print("Welcome");
  display.drawBitmap(39, 47, image_music_fast_forward_bits, 17, 16, 1);
  display.drawBitmap(17, 47, image_car_bits, 19, 16, 1);
  display.drawBitmap(104, 6, image_cloud_bits, 17, 16, 1);
  display.drawBitmap(7, 2, image_cloud_upload_2_bits, 17, 16, 1);
  display.drawBitmap(106, 40, image_earth_bits, 15, 16, 1);
  display.drawBitmap(59, 47, image_tree_bits, 16, 16, 1);
  display.drawBitmap(5, 47, image_toilets_gentlemen_bits, 7, 16, 1);
  display.drawBitmap(7, 39, image_weather_umbrella_bits, 15, 16, 1);
  display.drawBitmap(81, 8, image_weather_cloud_snow_bits, 17, 16, 1);
  display.drawBitmap(32, -1, image_weather_cloud_rain_bits, 17, 16, 1);
  display.drawBitmap(84, 44, image_plane_bits, 16, 16, 1);
  display.drawBitmap(58, 2, image_weather_cloud_sunny_bits, 17, 16, 1);
  display.display();
}