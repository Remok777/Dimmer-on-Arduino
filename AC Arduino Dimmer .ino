/******************************************************************
 * All what you need for this dimmer are:                         *
 *                                                                *
 * TFT Display ST7789                                             *
 * RTC module DS1302                                              *
 * AC Dimmer https://arduino.ua/prod2958-dimmer-220v-dlya-arduino *
 * 3 Buttons                                                      *
 *                                                                *
 * The schematic of connection:                                   *
 *                                                                *
 * 1)TFT Display ST7789:                                          *
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    SCL - 13 pin                                                *
 *    SDA - 11 pin                                                *
 *    RES - 9 pin                                                 *
 *    DC - 8 pin                                                  *
 * 2)RTC module DS1302:                                           *                                                               
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    RST - 7                                                     *
 *    DAT - 10                                                    *
 *    CLK -12                                                     *
 * 3)AC Dimmer:                                                   *       
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    m1 - 3                                                      *
 *    m2 - 2                                                      *
 * 4)3 Buttons                                                    *
 *   The first one to the 4 pin (Left button)                     *                                       
 *   The second one to the 5 pin (OK button)                      *
 *   The last one to the 6 pin (Right button)                     *
 *                                                                *
 ******************************************************************/

#include <Adafruit_GFX.h>
#include <Arduino_ST7789.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <TimeLib.h>
#include <stdio.h>
#include <DS1302.h>
#include <AC_Dimmer.h>
#include "GyverButton.h"

//DEFINING
#define LEFT  4
#define OK  5
#define RIGHT  6
#define TFT_DC    8
#define TFT_RST   9 
#define TFT_CS    10 
#define TFT_MOSI  11
#define TFT_SCLK  13
#define Dimmer_1 0

//BUTTONS
GButton buttLeft(LEFT);
GButton buttOk(OK);
GButton buttRight(RIGHT);


//MENU

volatile int currentMenu = 0, currentLine = 0;


//SCREEN
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);

//RTC
const int kCePin   = 7;  // Chip Enable
const int kIoPin   = 10;  // Input/Output
const int kSclkPin = 12;  // Serial Clock
DS1302 rtc(kCePin, kIoPin, kSclkPin);

float zatrymka = 0.0, x = 0.0;;
byte duration = 0, dimmduration = 0, hours_on1 = 0, minutes_on1 = 0, hours_off1 = 0, minutes_off1 = 0, hours_on2 = 0, minutes_on2 = 0, hours_off2 = 0, minutes_off2 = 0;
int i, j;
byte lightBrightness = 0, value_hours_on1 = 0, value_mins_on1 = 0;
void setup() {
  
  Dimmer_init_begin();
    Dimmer_pin_assign(Dimmer_1, 3); 
  Dimmer_init_end();
  
  //BUTTONS
  buttLeft.setType(HIGH_PULL);
  buttOk.setType(HIGH_PULL);
  buttRight.setType(HIGH_PULL);

  buttLeft.setDirection(NORM_OPEN);
  buttOk.setDirection(NORM_OPEN);
  buttRight.setDirection(NORM_OPEN);

  Serial.begin(9600);
  delay(300);
  tft.init(240, 240);
  uint16_t time = millis();
  tft.fillScreen(BLACK);
  time = millis() - time;

  tmElements_t tm;
  delay(500);
  Menu();
}

void loop() {
  tmElements_t tm;
  Time t = rtc.time();
  x = (255.0*float(lightBrightness))/100.0;
  dimmduration = EEPROM.read(0);
  dimmduration = dimmduration*60;
  zatrymka = (float(dimmduration)/x);
  zatrymka = zatrymka*1000.0;
  value_hours_on1 = EEPROM.read(1);
  value_mins_on1 = EEPROM.read(2);

  buttLeft.tick();
  buttOk.tick();
  buttRight.tick();

  if(value_hours_on1 == t.hr && value_mins_on1 == t.min){
    for(i = 0; i<=x; i++){
      Dimm_value(Dimmer_1, i);
      delay(zatrymka);
    }
  }
  if(EEPROM.read(3) == t.hr && EEPROM.read(4) == t.min){
    for(i = 0; i<=x; i++){
      Dimm_value(Dimmer_1, i);
      delay(zatrymka);
    }
  }
  if(EEPROM.read(5) == t.hr && EEPROM.read(6) == t.min){
    for(j = x; j >= 0; j--){
      Dimm_value(Dimmer_1, j);
      delay(zatrymka);
    }
  }
  if(EEPROM.read(7) == t.hr && EEPROM.read(8) == t.min){
    for(j = x; j >= 0; j--){
      Dimm_value(Dimmer_1, j);
      delay(zatrymka);
    }
  }
  
  if(currentMenu != 0){
    knopki();
  }

  if(currentMenu == 0 && currentLine == 0 && buttOk.isHold()){
    Menu();
    EEPROM.put(0, 0);
    EEPROM.put(1, 0);
    EEPROM.put(2, 0);
    EEPROM.put(3, 0);
    EEPROM.put(4, 0);
    EEPROM.put(5, 0);
    EEPROM.put(6, 0);
    EEPROM.put(7, 0);
    EEPROM.put(8, 0);
    EEPROM.put(9, 0);
  }

  if(currentMenu == 0){   
    if(buttRight.isClick()){
      currentLine++;
      lineChange();
    }
    if(buttLeft.isClick()){
      currentLine--;
      lineChange();
    }
    if(currentLine == 0 && buttOk.isClick()){
      currentMenu = 1;
      menuChange();
    }
    if(currentLine == 1 && buttOk.isClick()){
      currentMenu = 2;
      menuChange();
    }
    if(currentLine == 2 && buttOk.isClick()){
      currentMenu = 3;
      menuChange();
    }
    if(currentLine == 3 && buttOk.isClick()){
      currentMenu = 4;
      menuChange();
    }
    if(currentLine == 4 && buttOk.isClick()){
      currentMenu = 5;
      menuChange();
    }
    if(currentLine == 5 && buttOk.isClick()){
      currentMenu = 6;
      menuChange();
    }
    if(buttOk.isClick() && currentLine == 6){
    //Duration
    
    tft.fillScreen(BLACK);
    testdrawtext(WHITE, BLACK, 0, 20);
    tft.setTextSize(2);
    tft.println("Tryvalist:");
    testdrawtext(WHITE, BLACK, 50, 50);
    tft.setTextSize(3);
    tft.println(EEPROM.read(0));
    testdrawtext(WHITE, BLACK, 110, 20);
    tft.setTextSize(2);
    tft.println("Yaskravist:");
    testdrawtext(WHITE, BLACK, 150, 50);
    tft.setTextSize(3);
    tft.print(EEPROM.read(9));
    tft.print("%");

    //Turning on
    
    testdrawtext(WHITE, BLACK, 0, 80);
    tft.setTextSize(2);
    tft.println("Chas vvimknennya:");
    testdrawtext(WHITE, BLACK, 70, 110);
    tft.setTextSize(2);
    tft.print("1 - ");
    tft.print(EEPROM.read(1));
    tft.print(":");
    tft.print(EEPROM.read(2));
    testdrawtext(WHITE, BLACK, 70, 135);
    tft.setTextSize(2);
    tft.print("2 - ");
    tft.print(EEPROM.read(3));
    tft.print(":");
    tft.print(EEPROM.read(4));
    testdrawtext(WHITE, BLACK, 0, 160);
    tft.setTextSize(2);
    tft.println("Chas vymknennya:");
    testdrawtext(WHITE, BLACK, 70, 190);
    tft.setTextSize(2);
    tft.print("1 - ");
    tft.print(EEPROM.read(5));
    tft.print(":");
    tft.print(EEPROM.read(6));
    testdrawtext(WHITE, BLACK, 70, 190);
    tft.setTextSize(2);
    tft.print("1 - ");
    tft.print(EEPROM.read(5));
    tft.print(":");
    tft.print(EEPROM.read(6));
    testdrawtext(WHITE, BLACK, 70, 215);
    tft.setTextSize(2);
    tft.print("2 - ");
    tft.print(EEPROM.read(7));
    tft.print(":");
    tft.print(EEPROM.read(8));
  
  }
  else if(buttOk.isHold()){
    Menu();
  }
  
 }
    
}
void testdrawtext(uint16_t color, uint16_t bg, int x, int y) { 
  tft.setCursor(x, y);
  tft.setTextColor(color, bg);
  tft.setTextWrap(true);
  tft.setTextSize(3);
}

void Menu(){
  if(currentMenu == 0){
    Serial.println(currentMenu);
    currentLine = 0;
    
    tft.fillScreen(BLACK);
    lineChange();
  }
}
void buttMenu(){
  buttLeft.tick();
  buttOk.tick();
  buttRight.tick();
  
 if (buttLeft.isClick()) {
   
      if (currentLine > 0) {
        currentLine--;
      } 
      else {
        currentLine = 6;
      }

  if (buttRight.isClick()){
    
      if (currentLine < 6) {
        currentLine++;
      }
      else{
        currentLine = 0;
      }
      
    }
 } 
  delay(50);
}
void lineChange(){
    Time t = rtc.time();
    if(currentLine == 0){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(BLACK, WHITE, 0, 30);
      tft.println("Duration");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on  1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
    }
    if(currentLine == 1){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duration");
      testdrawtext(BLACK, WHITE, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
    }
    if(currentLine == 2){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duration");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(BLACK, WHITE, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
    }
    if(currentLine == 3){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duration");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(BLACK, WHITE, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
      
    }
    if(currentLine == 4){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duraion");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(BLACK, WHITE, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
    }
    if(currentLine == 5){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duration");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(BLACK, WHITE, 0, 180);
      tft.println("Brigtness");
      testdrawtext(WHITE, BLACK, 0, 210);
      tft.println("All options");
    }
    if(currentLine == 6){
      testdrawtext(WHITE, BLACK, 180, 0);
      tft.setTextSize(2);
      tft.print(t.hr);
      tft.print(":");
      tft.print(t.min);
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 0);
      
      delay(1);
      testdrawtext(WHITE, BLACK, 0, 30);
      tft.println("Duration");
      testdrawtext(WHITE, BLACK, 0, 60);
      tft.println("Turn on 1");
      testdrawtext(WHITE, BLACK, 0, 90);
      tft.println("Turn on 2");
      testdrawtext(WHITE, BLACK, 0, 120);
      tft.println("Turn off 1");
      testdrawtext(WHITE, BLACK, 0, 150);
      tft.println("Turn off 2");
      testdrawtext(WHITE, BLACK, 0, 180);
      tft.println("Brightness");
      testdrawtext(BLACK, WHITE, 0, 210);
      tft.println("All options");
    }
}
void menuChange(){
  if (currentMenu == 1){ 
    tft.fillScreen(BLACK); 
    testdrawtext(WHITE, BLACK, 100, 100);
    tft.setTextSize(4);
    tft.println(duration);
  }
  if(currentMenu == 2){
    tft.fillScreen(BLACK);
    testdrawtext(WHITE, BLACK, 80, 100);
    tft.setTextSize(4);
    tft.print(hours_on1);
    tft.print(":");
    tft.print(minutes_on1);
  }
  if(currentMenu == 3){
    tft.fillScreen(BLACK);
    testdrawtext(WHITE, BLACK, 80, 100);
    tft.setTextSize(4);
    tft.print(hours_on2);
    tft.print(":");
    tft.print(minutes_on2);
  }
  if(currentMenu == 4){
    tft.fillScreen(BLACK);
    testdrawtext(WHITE, BLACK, 80, 100);
    tft.setTextSize(4);
    tft.print(hours_off1);
    tft.print(":");
    tft.print(minutes_off1);
  }
  if(currentMenu == 5){
    tft.fillScreen(BLACK);
    testdrawtext(WHITE, BLACK, 80, 100);
    tft.setTextSize(4);
    tft.print(hours_off2);
    tft.print(":");
    tft.print(minutes_off2);
  }
  if (currentMenu == 6){ 
    tft.fillScreen(BLACK); 
    testdrawtext(WHITE, BLACK, 100, 100);
    tft.setTextSize(4);
    tft.println(lightBrightness);
  }
}
void knopki(){
  if(buttRight.isHold() || buttRight.isClick()){
    if(currentMenu == 1){
      delay(10);
      duration++;
      Serial.println(duration);
      testdrawtext(WHITE, BLACK, 100, 100);
      tft.setTextSize(4);
      tft.println(duration);
      if(duration > 59){
        duration = 0;
      }
    }
    if(currentMenu == 2){
      minutes_on1 = minutes_on1 + 5;
      if(minutes_on1 > 59){
        hours_on1++;
        minutes_on1 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_on1);
      tft.print(":");
      tft.print(minutes_on1);
    }
    if(currentMenu == 3){
      minutes_on2 = minutes_on2 + 5;
      if(minutes_on2 > 59){
        hours_on2++;
        minutes_on2 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_on2);
      tft.print(":");
      tft.print(minutes_on2);
    }
    if(currentMenu == 4){
      minutes_off1 = minutes_off1 + 5;
      if(minutes_off1 > 59){
        hours_off1++;
        minutes_off1 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_off1);
      tft.print(":");
      tft.print(minutes_off1);
    }
    if(currentMenu == 5){
      minutes_off2 = minutes_off2 + 5;
      if(minutes_off2 > 59){
        hours_off2++;
        minutes_off2 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_off2);
      tft.print(":");
      tft.print(minutes_off2);
    }
    if(currentMenu == 6){
      delay(10);
      lightBrightness = lightBrightness + 5;
      Serial.println(lightBrightness);
      testdrawtext(WHITE, BLACK, 100, 100);
      tft.setTextSize(4);
      tft.print(lightBrightness);
      tft.print("%");
      if(lightBrightness >= 100){
        lightBrightness = 0;
      }
    }
  }
  if(buttLeft.isHold() || buttLeft.isClick()){
    if(currentMenu == 1){
      delay(10);
      duration--;
      Serial.println(duration);
      testdrawtext(WHITE, BLACK, 100, 100);
      tft.setTextSize(4);
      tft.println(duration);
      if(duration >= 60 || duration == 255){
        duration = 60;
      }
    }
    if(currentMenu == 2){
      minutes_on1 = minutes_on1 - 5;
      if(minutes_on1 >= 60 ){
        minutes_on1 = 0;
        hours_on1--;
      }
      if(hours_on1 > 23){
        hours_on1 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_on1);
      tft.print(":");
      tft.print(minutes_on1);
    }
    if(currentMenu == 3){
      minutes_on2 = minutes_on2 - 5;
      if(minutes_on2 >= 60 ){
        minutes_on2 = 0;
        hours_on2--;
      }
      if(hours_on2 > 23){
        hours_on2 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_on2);
      tft.print(":");
      tft.print(minutes_on2);
    }
    if(currentMenu == 4){
      minutes_off1 = minutes_off1 - 5;
      if(minutes_off1 >= 60 ){
        minutes_off1 = 0;
        hours_off1--;
      }
      if(hours_off1 > 23){
        hours_off1 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_off1);
      tft.print(":");
      tft.print(minutes_off1);
    }
    if(currentMenu == 5){
      minutes_off2 = minutes_off2 - 5;
      if(minutes_off2 >= 60 ){
        minutes_off2 = 0;
        hours_off2--;
      }
      if(hours_off2 > 23){
        hours_off2 = 0;
      }
      testdrawtext(WHITE, BLACK, 80, 100);
      tft.setTextSize(4);
      tft.print(hours_off2);
      tft.print(":");
      tft.print(minutes_off2);
    }
    if(currentMenu == 6){
      delay(10);
      lightBrightness = lightBrightness - 5;
      Serial.println(lightBrightness);
      testdrawtext(WHITE, BLACK, 100, 100);
      tft.setTextSize(4);
      tft.print(lightBrightness);
      tft.print("%");
      if(lightBrightness >= 100 || lightBrightness == 255){
        lightBrightness = 100;
      }
    }
  }
  if(buttOk.isClick()){
    if(currentMenu == 1){
      EEPROM.put(0, duration);
      currentMenu--;
      Menu();
    }
    if(currentMenu == 2){
      EEPROM.put(1, hours_on1);
      EEPROM.put(2, minutes_on1);
      currentMenu--;
      Menu();
    }
    if(currentMenu == 3){
      EEPROM.put(3, hours_on2);
      EEPROM.put(4, minutes_on1);
      currentMenu--;
      Menu();
    }
    if(currentMenu == 4){
      EEPROM.put(5, hours_off1);
      EEPROM.put(6, minutes_off1);
      currentMenu--;
      Menu();
    }
    if(currentMenu == 5){
      EEPROM.put(7, hours_off2);
      EEPROM.put(8, minutes_off2);
      currentMenu--;
      Menu();
    }
    if(currentMenu == 6){
      EEPROM.put(9, lightBrightness);
      currentMenu--;
      Menu();
    }
    
  }
  
}
