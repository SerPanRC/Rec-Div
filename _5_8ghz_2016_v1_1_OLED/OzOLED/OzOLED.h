/*
  OzOLED.h - 0.96' I2C 128x64 OLED Driver Library
  2014 Copyright (c) OscarLiang.net  All right reserved.
 
  Author: Oscar Liang
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/

#ifndef OzOLED_data_H
#define OzOLED_data_H

#include <Arduino.h>

#define OzOLED_Max_X					128	//128 Pixels
#define OzOLED_Max_Y					64	//64  Pixels

#define OLED_ADDRESS					0x3C
#define I2C_400KHZ						1	// 0 to use default 100Khz, 1 for 400Khz

// registers
#define OzOLED_COMMAND_MODE				0x80
#define OzOLED_DATA_MODE				0x40

// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define ASA_DISPLAY_ALL_ON_RESUME            0xA4
// Addressing Setting Commands
#define ASA_MEMORY_ADDR_MODE                0x20
// Hardware Configuration Commands
#define ASA_SET_START_LINE                0x40
#define ASA_SET_SEGMENT_REMAP                0xA0
#define ASA_SET_MULTIPLEX_RATIO                0xA8
#define ASA_COM_SCAN_DIR_DEC            0xC8
#define ASA_SET_DISPLAY_OFFSET            0xD3
#define ASA_SET_COM_PINS                0xDA
#define ASA_CHARGE_PUMP               0x8D
// Timing & Driving Scheme Setting Commands
#define ASA_SET_DISPLAY_CLOCK_DIV_RATIO        0xD5
#define ASA_SET_PRECHARGE_PERIOD        0xD9
#define ASA_SET_VCOM_DESELECT                0xDB


// commands
#define OzOLED_CMD_DISPLAY_OFF			0xAE
#define OzOLED_CMD_DISPLAY_ON			0xAF
#define OzOLED_CMD_NORMAL_DISPLAY		0xA6
#define OzOLED_CMD_INVERSE_DISPLAY		0xA7
#define OzOLED_CMD_SET_BRIGHTNESS		0x81

#define OzOLED_RIGHT_SCROLL				0x26
#define OzOLED_LEFT_SCROLL				0x27
#define OzOLED_SET_VERTICAL_SCROLL_AREA 0xA3
#define OzOLED_VERTICAL_RIGHT_SCROLL	0x29
#define OzOLED_VERTICAL_LEFT_SCROLL		0x2A
#define OzOLED_CMD_ACTIVATE_SCROLL		0x2F
#define OzOLED_CMD_DEACTIVATE_SCROLL	0x2E

#define HORIZONTAL_ADDRESSING	0x00
#define PAGE_ADDRESSING			0x02

#define Scroll_Left				0x00
#define Scroll_Right			0x01
#define Scroll_Up				0x02
#define Scroll_Down				0x03

#define Scroll_2Frames			0x07
#define Scroll_3Frames			0x04
#define Scroll_4Frames			0x05
#define Scroll_5Frames			0x00
#define Scroll_25Frames			0x06
#define Scroll_64Frames			0x01
#define Scroll_128Frames		0x02
#define Scroll_256Frames		0x03


class OzOLED {

public:

	byte addressingMode;

	void sendCommand(byte command);
	void sendData(byte Data);

     // исходные функции - фонт 8х8
    void printChar(char c, byte X=255, byte Y=255);
    void printString(const char *String, byte X=255, byte Y=255, byte numChar=255);
// новые функции - фонт 16х16
    void printChar16(char c, byte X=255, byte Y=255);    // font 16x16
    void printString16(const char *String, byte X, byte Y, byte numChar=255);    // font 16x16
// новые функции - фонт 12х16, вариант a
    void printChar12a(char c, byte X=255, byte Y=255);    // font 12x16
    void printString12a(const char *String, byte X, byte Y, byte numChar=255);    // font 12x16
// новые функции - фонт 12х16, вариант b
    void printChar12b(char c, byte X=255, byte Y=255);    // font 12x16
    void printString12b(const char *String, byte X, byte Y, byte numChar=255);    // font 12x16
// далее - старые функции - их аналогов делать не стал, если кому понадоб€тс€, можно сделать по образу и подобию имеющихс€
    byte printNumber(long n, byte X=255, byte Y=255);
    byte printNumber(float float_num, byte prec=6, byte Y=255, byte numChar=255);
    void printBigNumber(const char *number, byte column=0, byte page=0, byte numChar=255);
    void drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height);
void VertBar(int Num, int Val, int MinVal, int MaxVal); // draw vertical line from bottom, h ~ (Val - MinVal)/(MaxVal - MinVal)

// новы€ функци€, позиционирующа€ по X с шагом 4 пиксел€ - нужна дл€ шрифта 12х16 (исходна€ setCursorXY позиционирует с шагом 8 - целый символ 8х8)
    void setCursorX2Y(byte Column, byte Row); // X * 4 pixels, Y * 8 pixels 

   


	
	void init();
	
	void setCursorXY(byte Column, byte Row);
            void setCursorXY_L(byte Column, byte Row);
	void clearDisplay();
	//void clearPage(byte page);
	
	void setNormalDisplay();
	void setInverseDisplay();
	void setPowerOff();
	void setPowerOn();
	void setPageMode();
	void setHorizontalMode();
	void setBrightness(byte Brightness);
	
	void scrollRight(byte start, byte end, byte speed);
	void scrollLeft(byte start, byte end, byte speed);
	void scrollDiagRight();
	void scrollDiagLeft();
	void setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed);
	void setDeactivateScroll();

};

extern OzOLED OzOled;  // OzOLED object 

#endif


