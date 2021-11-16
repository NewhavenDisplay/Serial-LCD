
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

uint8_t _SCL;
uint8_t _RS;
uint8_t _SDI;
uint8_t _CS;
uint8_t _RESET;

uint8_t contrast = 0x20;

uint8_t customChar[] = {
  0b00011111,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00011111,
  0b00000000,
  0b00000000
};

void setup() 
{
  Serial.begin(9600);

  initCOG(7, 6, 5, 4, 3);

  setCustomCharacter();
  write(0x00);
  
}

void loop() 
{
  /*
  setContrast(contrast);
  delay(300);
  contrast--;

  if(contrast < 0x10)
  {
    contrast = 0x20;
  }
  */
}

void initCOG(uint8_t SCL, uint8_t RS, uint8_t SDI, uint8_t CS ,uint8_t RESET)
{
  _SCL = SCL;
  _RS = RS;
  _SDI = SDI;
  _CS = CS;
  _RESET = RESET;

  pinMode(CS, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(RS, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(RESET, OUTPUT);

  digitalWrite(CS, HIGH);
  digitalWrite(SCL, HIGH);
  digitalWrite(RESET, HIGH);

  reset();

  command(0x30); // wakeup
  delay(2);
  command(0x30); // wakeup
  command(0x30); // wakeup
  command(0x39); // function set
  command(0x14); // internal osc frequency
  command(0x56); // power control
  command(0x6D); // follower control
  command(0x70); // contrast
  command(0x0C); // display ON
  command(0x06); // entry mode
  command(0x01); // clear screen
  delay(10);
}

void reset()
{
  digitalWrite(_RESET, LOW);
  delay(2);
  digitalWrite(_RESET, HIGH);
  delay(20);
}

void setCS()
{
  digitalWrite(_CS, HIGH);
}

void clearCS()
{
  digitalWrite(_CS, LOW);
}

void setCommand()
{
  // Write RS
  digitalWrite(_RS, LOW);
}

void setData()
{
  // Write RS
  digitalWrite(_RS, HIGH);
}

void command(uint8_t data)
{
  clearCS();
  delay(1);

  setCommand();
  
  putData(data);
  
  setCS();
}

void write(uint8_t data)
{
  clearCS();
  delay(1);

  setData();
  
  putData(data);
  
  setCS();
  delay(1);
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

void functionSet(uint8_t interfaceData, uint8_t lines, uint8_t doubleHeight, uint8_t instructionTable)
{
  command(0x20 | interfaceData | lines | doubleHeight | instructionTable);
}

void setContrast(uint8_t contrast)
{
  uint8_t contrastLower = contrast & 0x0F;
  uint8_t contrastUpper = (contrast >> 4) & 0x03;

  functionSet(DATA_8BIT, LINES_1, DOUBLEHEIGHT_ON, EXTENDED_INSTRUCTION_TABLE_ON);
  command(0x50 | 0x04 | contrastUpper); // Power control 
  command(0x70 | contrastLower); // Set contast
}

void setDoubleHeight()
{
  functionSet(DATA_8BIT, LINES_1, DOUBLEHEIGHT_ON, EXTENDED_INSTRUCTION_TABLE_OFF);
  
}

void setCGRAM(uint8_t address)
{
  command(0x40 | (address & 0x3F));
}

void setCustomCharacter()
{
  
  functionSet(DATA_8BIT, LINES_2, DOUBLEHEIGHT_OFF, EXTENDED_INSTRUCTION_TABLE_OFF);
  setCGRAM(0);
  for(int i = 0; i < 8; i++)
  {
    
    write(customChar[i]);
  }
  // Set CGRAM address to 0x00
  // Write byte of character data to CGRAM
  delay(1000);
  functionSet(DATA_8BIT, LINES_2, DOUBLEHEIGHT_OFF, EXTENDED_INSTRUCTION_TABLE_OFF);
  command(0x01);
  delay(2);
  command(0x02);
  delay(5);
}

void drawCustomCharacter()
{
  
}