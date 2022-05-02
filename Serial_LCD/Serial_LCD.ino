//---------------------------------------------------------
/*
Serial_LCD.ino
This code was written to interface and Arduino UNO with NHD-0420D3Z-FL-GBW-V3.

(c)2021 Cody Johnson - Newhaven Display International, LLC.

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
*/
//---------------------------------------------------------


//---------------------------------------------------------
/*
I2C Wiring Reference:

- Arduino Pin 5 (SCL) to LCD J2 Pin 3
- Arduino Pin 4 (SDA) to LCS J2 Pin 4
- GND to LCD J2 Pin 5
- 5V to LCD J2 Pin 6

SPI Wiring Reference:

- Arduino Pin 5 (SCL) to LCD J2 Pin 3
- Arduino Pin 4 (SDI) to LCD J2 Pin 4
- Arduino Pin 3 (/CS) to LCD J2 Pin 1
- GND to LCD J2 Pin 5
- 5V to LCD J2 Pin 6

RS232 Wiring Reference:

- Arduino Pin 2 (TX) to LCD J1 Pin 1
- GND to LCD J1 Pin 2
- 5V to LCD J1 Pin 3

*/
//---------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>

#define STARTUP_DELAY 500

#define RS232_DELAY 100

#define I2C_DELAY 100
#define SLAVE_ADDRESS 0x28

// SPI Interface
uint8_t _SCL; // 5
uint8_t _SDI; // 4
uint8_t _CS; // 3

// RS232 Interface
uint8_t _RX; // 2

//I2C Interface
uint8_t _SDA; // 4

enum Interface{
  I2C,
  SPI,
  RS232
};

Interface interface;

void setup() 
{
  //initLCD_I2C(5, 4);
  initLCD_SPI(5, 4, 3);
  //initLCD_RS232(2);

  writeString((unsigned char*)"Newhaven ");
  //setBrightness(8);
}

void loop() 
{

}

void initLCD_I2C(uint8_t SCL, uint8_t SDA)
{
  interface = I2C;

  _SCL = SCL;
  _SDA = SDA;

  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);
  
  digitalWrite(SCL, HIGH);
  digitalWrite(SDA, HIGH);
  
  delay(STARTUP_DELAY);

  clearScreen();
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
void initLCD_SPI(uint8_t SCL, uint8_t SDI, uint8_t CS)
{
  interface = SPI;
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
  delay(STARTUP_DELAY);

  clearScreen();
}

void initLCD_RS232(uint8_t RX)
{
  interface = RS232;

  _RX = RX;

  pinMode(RX, OUTPUT);
  digitalWrite(RX, HIGH);

  delay(STARTUP_DELAY);
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

void startBit()
{
  digitalWrite(_RX, LOW);
  delayMicroseconds(RS232_DELAY);
}

void stopBit()
{
  digitalWrite(_RX, HIGH);
  delayMicroseconds(RS232_DELAY);
}

void startCondition()
{
  clearSDA();
  clearSCL();
}

void stopCondition()
{
  setSCL();
  setSDA();
}

void setSDA()
{
  digitalWrite(_SDA, HIGH);
  delayMicroseconds(I2C_DELAY);
}

void clearSDA()
{
  digitalWrite(_SDA, LOW);
  delayMicroseconds(I2C_DELAY);
}

void setSCL()
{
  digitalWrite(_SCL, HIGH);
  delayMicroseconds(I2C_DELAY);
}

void clearSCL()
{
  digitalWrite(_SCL, LOW);
  delayMicroseconds(I2C_DELAY);
}

void setWriteMode()
{
  //putData_I2C((SLAVE_ADDRESS << 1) | 0x00);
  putData_I2C(0x50);
}

void setReadMode()
{
  putData_I2C((SLAVE_ADDRESS << 1) | 0x01);
}

uint8_t getACK()
{
  pinMode(_SDA, INPUT);
  setSCL();

  uint8_t ACK = digitalRead(_SDA);

  pinMode(_SDA, OUTPUT);
  clearSCL();

  return ACK;
}

/**
 * @brief Write 1 byte of data to the display.
 * 
 * @param data Byte of data to be written.
 * @return none
 */
void write(uint8_t data)
{
  switch(interface)
  {
    case I2C:
      startCondition();
      setWriteMode();
      putData_I2C(data);
      stopCondition();
      break;
    case SPI:
      clearCS();
      putData_SPI(data);
      setCS();
      break;
    case RS232:
      startBit();
      putData_RS232(data);
      stopBit();
      delay(1);
      break;
    default:
      break;
  }
  delayMicroseconds(150);
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

void putData_I2C(uint8_t data)
{
  for(int i = 7; i >= 0; i--)
  {
    digitalWrite(_SDA, (data >> i) & 0x01);

    setSCL();
    clearSCL();
  }

  getACK();
}

/**
 * @brief Put each bit of data on the SPI data bus.
 * This function sends MSB (D7) first and LSB (D0) last.
 * 
 * @param data Byte of data to be put on the SPI data bus.
 * @return none
 */
void putData_SPI(uint8_t data)
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
 * @brief Put each bit of data on the RS232 data bus.
 * This function sends LSB (D0) first and MSB (D7) last.
 * 
 * @param data Byte of data to be put on the RS232 data bus.
 * @return none
 */
void putData_RS232(uint8_t data)
{
  // Write data byte LSB first -> MSB last
  for(int i = 0; i <= 7; i++)
  {
    digitalWrite(_RX, (data >> i) & 0x01);
    delayMicroseconds(RS232_DELAY);
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
  write(0x45);
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
  delay(2);
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