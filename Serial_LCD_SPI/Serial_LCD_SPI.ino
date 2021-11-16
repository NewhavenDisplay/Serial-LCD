
/**
 * This code was written to interface via SPI with NHD-C0216CS-FSW-FBW-3V3.
 * 
 * Wiring Reference:
 * 
 * SCL to OLED 7
 * RS to OLED 6
 * SDI (MOSI) to 
 * /CS to 
 */

/**
 * TODO:
 * - CGRAM (customer character)
 */

#include <stdint.h>

#define FUNTION_SET 0x20
#define DATA_8BIT 0x10
#define DATA_4BIT 0x00
#define LINES_2 0x08
#define LINES_1 0x00
#define DOUBLEHEIGHT_ON 0x04
#define DOUBLEHEIGHT_OFF 0x00
#define EXTENDED_INSTRUCTION_TABLE_ON 0x01
#define EXTENDED_INSTRUCTION_TABLE_OFF 0x00

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