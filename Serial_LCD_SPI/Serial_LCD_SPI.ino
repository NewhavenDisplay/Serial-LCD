
/**
 * This code was written to interface via SPI with NHD-0420D3Z-FL-GBW-V3.
 * 
 * Wiring Reference:
 * 
 * SCL to LCD J2 Pin 3
 * SDI (MOSI) to LCD J2 Pin 4
 * /CS to LCD J2 Pin 1
 * GND to LCD J2 Pin 5
 * 5V to LCD J2 Pin 6
 */

#include <stdint.h>

uint8_t _SCL; // 5
uint8_t _SDI; // 4
uint8_t _CS; // 3

void setup() 
{
  Serial.begin(9600);

  initLCD(5, 4, 3);
  write('A');
  
}

void loop() 
{

}

void initLCD(uint8_t SCL, uint8_t SDI, uint8_t CS)
{
  _SCL = SCL;
  _SDI = SDI;
  _CS = CS;

  pinMode(CS, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDI, OUTPUT);

  digitalWrite(CS, HIGH);
  digitalWrite(SCL, HIGH);

  delay(500);

  // Init
  displayON();
  clearScreen();
}

void setCS()
{
  digitalWrite(_CS, HIGH);
  delay(1);
}

void clearCS()
{
  digitalWrite(_CS, LOW);
  delay(1);
}

void write(uint8_t data)
{
  clearCS();
  putData(data);
  setCS();
}

void putData(uint8_t data)
{
  // Write data byte
  for(int i = 7; i >= 0; i--)
  {
    digitalWrite(_SCL, LOW);

    digitalWrite(_SDI, (data >> i) & 0x01);
    
    delayMicroseconds(1);
    digitalWrite(_SCL, HIGH);
  }
}

void prefix()
{
  write(0xFE);
}

void displayON()
{
  prefix();
  write(0x41);
}

void displayOFF()
{
  prefix();
  write(0x42);
}

void setCursor(uint8_t position)
{
  prefix();
  write(position);
}

void home()
{
  prefix();
  write(0x46);
}

void clearScreen()
{
  prefix();
  write(0x51);
}

void setContrast(uint8_t contrast)
{
  prefix();
  write(0x52);
  write(contrast);
}

void setBrightness(uint8_t brightness)
{
  prefix();
  write(0x53);
  write(brightness);
}

void underlineCursorON()
{
  prefix();
  write(0x47);
}