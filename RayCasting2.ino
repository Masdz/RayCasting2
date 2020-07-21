#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#define PINY 14
#define PINX 15
#define PINZ 16

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int readjoy(byte pin, byte max){
  return map(analogRead(pin),0,1000,-max,max);
}

void setup() {
  u8g2.begin();
  pinMode(PINX,INPUT);
  pinMode(PINY,INPUT);
  pinMode(PINZ,INPUT);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(0, 15);
  u8g2.print(readjoy(PINX,20));
  u8g2.setCursor(0, 32);
  u8g2.print(readjoy(PINY,5));
  u8g2.setCursor(0, 48);
  u8g2.print(digitalRead(PINZ));
  u8g2.sendBuffer();
}
