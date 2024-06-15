#include <Arduino.h>
#include <ESP8266WiFi.h>  // Подключаем библиотеку ESP8266WiFi
#include <Wire.h>
#include "display.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void oled_init() {
  sendCommand(OLED_CMD_DISPLAY_OFF);
  sendCommand(SSD1306_CHARGE_PUMP);
  sendCommand(0x14);

  sendCommand(0x20);             //set addressing mode
  sendCommand(PAGE_ADDRESSING);  //set page addressing mode
  sendCommand(OLED_CMD_DISPLAY_ON);
  sendCommand(OLED_CMD_NORMAL_DISPLAY);
  //rotate 180
  sendCommand(0xA1);  //sendCommand(0xA0);a1
  sendCommand(0xC8);  // sendCommand(0xC0)c8

  sendCommand(OLED_CMD_SET_BRIGHTNESS);
  sendCommand(0xEf);  // яркость
  clearDisplay();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setCursorXY(uint8_t Xs, uint8_t Ys) {
  // Y - 1 unit = 1 page (8 pixel rows)
  // X - 1 unit = 8 pixel columns
  sendCommand(0x00 + (8 * Xs & 0x0F));         //set column lower address
  sendCommand(0x10 + ((8 * Xs >> 4) & 0x0F));  //set column higher address
  sendCommand(0xB0 + Ys);                      //set page address
}

// ====================== LOW LEVEL =====//////////////////////////////////////////////////
void sendCommand(uint8_t command) {
  Wire.beginTransmission(OLED_ADDRESS);  // begin transmitting
  Wire.write(OLED_COMMAND_MODE);         //data mode
  Wire.write(command);
  Wire.endTransmission();  // stop transmitting
}

//////////////////////////////////////////////////////////////////////////////////////////////
void sendData(uint8_t data) {
  Wire.beginTransmission(OLED_ADDRESS);  // begin transmitting
  Wire.write(OLED_DATA_MODE);            //data mode
  Wire.write(data);
  Wire.endTransmission();  // stop transmitting
}

///////////////////////////////////////////////////////////////////////////////////////////////
void clearDisplay() {
  setCursorXY(0, 0);
  for (uint8_t page = 0; page < 8; page++) {
    setCursorXY(0, page);
    for (uint8_t column = 0; column < 130; column++) {  //clear all columns
      sendData(0x00);
      sendData(0x00);
    }
  }
  setCursorXY(0, 0);
}


//////////////////////////////////////////////////////////////////////////////////////////////
void printChar(unsigned char C, uint8_t Xs, uint8_t Ys) {
  if (Xs < 131) setCursorXY(Xs, Ys);

  for (uint8_t i = 0; i < 5; i++) {
    uint8_t bb = pgm_read_byte(&BasicFont[C - 32][i]);
    sendData(bb);  //font array starts at 0, ASCII starts at 32. Hence the translation
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
void printString(String mstr, uint8_t Xs, uint8_t Ys) {
  if (Xs < 131) setCursorXY(Xs, Ys);
  uint8_t count = 0;
  while (mstr[count] && count < mstr.length()) {
    uint8_t tt = mstr[count];
    if (tt != 208 && tt != 209) {
      printChar(tt, Xs, Ys);
      Xs = Xs + 1;
    }
    count++;
  }
}

