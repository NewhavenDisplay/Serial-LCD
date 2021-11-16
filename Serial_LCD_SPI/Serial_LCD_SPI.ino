//---------------------------------------------------------
/*
Serial_LCD_SPI.ino
This code was written to interface and Arduino UNO via SPI with NHD-0420D3Z-FL-GBW-V3.

(c)2021 Cody Johnson - Newhaven Display International, LLC.

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

Wiring Reference:

SCL to LCD J2 Pin 3
SDI (MOSI) to LCD J2 Pin 4
/CS to LCD J2 Pin 1
GND to LCD J2 Pin 5
5V to LCD J2 Pin 6

*/
//---------------------------------------------------------

#include <stdint.h>

uint8_t _SCL; // 5
uint8_t _SDI; // 4
uint8_t _CS; // 3

void setup() 
{
  initLCD(5, 4, 3);
  writeString((unsigned char*)"Newhaven");
}

void loop() 
{

}

/**
 * @brief Initialize selected IO ports, wait for the display to power ON,
 * and clear the screen.
 * 
 * @param SCL Serial clock pin assignment.
 * @param SDI Serial data pin assignment.
 * @param CS Chip/Slave select pin assignment.
 * @return none
 */
void initLCD(uint8_t SCL, uint8_t SDI, uint8_t CS)
{
  // Store pin assignments globally
  _SCL = SCL;
  _SDI = SDI;
  _CS = CS;

  // Set IO modes
  pinMode(CS, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDI, OUTPUT);

  // Set pin states
  digitalWrite(CS, HIGH);
  digitalWrite(SCL, HIGH);

  // Wait for display to power ON.
  delay(500);

  clearScreen();
}

/**
 * @brief Set chip/slave select HIGH and wait for 1ms.
 * 
 * @return none
 */
void setCS()
{
  digitalWrite(_CS, HIGH);
  delay(1);
}

/**
 * @brief Clear chip/slave select and wait for 1ms.
 * 
 * @return none
 */
void clearCS()
{
  digitalWrite(_CS, LOW);
  delay(1);
}

/**
 * @brief Write 1 byte of data to the display.
 * 
 * @param data Byte of data to be written.
 * @return none
 */
void write(uint8_t data)
{
  clearCS();
  putData(data);
  setCS();
}

/**
 * @brief Write an array of characters to the display.
 * 
 * @param data Pointer to the array of characters.
 * @return none
 */
void writeString(unsigned char* data)
{
  // Iterate through data until null terminator is found.
  while(*data != '\0')
  {
    write(*data);
    data++; // Increment pointer.
  }
}

/**
 * @brief Put each bit of data on the data bus.
 * This function sends MSB (D7) first and LSB (D0) last.
 * 
 * @param data Byte of data to be put on the data bus.
 * @return none
 */
void putData(uint8_t data)
{
  // Write data byte MSB first -> LSB last
  for(int i = 7; i >= 0; i--)
  {
    digitalWrite(_SCL, LOW);

    digitalWrite(_SDI, (data >> i) & 0x01);
    
    delayMicroseconds(1);
    digitalWrite(_SCL, HIGH);
  }
}

/**
 * @brief Send the prefix data byte (0xFE).
 * 
 * @return none
 */
void prefix()
{
  write(0xFE);
}

/**
 * @brief Turn the display ON.
 * Display is turned ON by default.
 * 
 * @return none
 */
void displayON()
{
  prefix();
  write(0x41);
}

/**
 * @brief Turn the display OFF.
 * Display is turned ON by default.
 * 
 * @return none
 */
void displayOFF()
{
  prefix();
  write(0x42);
}

/**
 * @brief Set the display cursor position via DDRAM address.
 * 
 * @param position Desired DDRAM address.
 * @return none
 */
void setCursor(uint8_t position)
{
  prefix();
  write(position);
}

/**
 * @brief Move the cursor to line 1, column 1.
 * 
 * @return none
 */
void home()
{
  prefix();
  write(0x46);
}

/**
 * @brief Clear the display screen.
 * 
 * @return none
 */
void clearScreen()
{
  prefix();
  write(0x51);
}

/**
 * @brief Set the display's contrast.
 * 0x00 <= contrast <= 0x32
 * Default: 0x28
 * 
 * @param contrast Desired contrast setting.
 * @return none 
 */
void setContrast(uint8_t contrast)
{
  prefix();
  write(0x52);
  write(contrast);
}

/**
 * @brief Set the display's brightness.
 * 0x01 <= brightness <= 0x08
 * brightness = 0x01 | Backlight OFF
 * brightness = 0x08 | Backlight ON (100%)
 * 
 * @param brightness Desired brightness setting.
 * @return none
 */
void setBrightness(uint8_t brightness)
{
  prefix();
  write(0x53);
  write(brightness);
}

/**
 * @brief Turn the underline cursor ON.
 * 
 * @return none
 */
void underlineCursorON()
{
  prefix();
  write(0x47);
}