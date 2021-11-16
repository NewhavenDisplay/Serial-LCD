#include <stdint.h>

#define SLAVE_ADDRESS 0x28

uint8_t _SCL; // 5
uint8_t _SDA; // 4

void setup()
{
  Serial.begin(9600);

  initOLED_I2C(5, 4);
  write('A');
  write('A');
}

void loop()
{

}

void initOLED_I2C(uint8_t SCL, uint8_t SDA)
{
  _SCL = SCL;
  _SDA = SDA;

  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);

  digitalWrite(SCL, HIGH);

  delay(1);

  displayON();
  clearScreen();

}

// Send MSB first
// Change SDA only when SCL is LOW

void write(uint8_t data)
{
  startCondition();
  setWriteMode();
  putData(data);
  stopCondiiton();
}

void startCondition()
{
  digitalWrite(_SDA, LOW);
  delayMicroseconds(50);
  digitalWrite(_SCL, LOW);
}

void stopCondiiton()
{
  digitalWrite(_SCL, HIGH);
  delayMicroseconds(50);
  digitalWrite(_SDA, HIGH);
}

void setSCL()
{
  digitalWrite(_SCL, HIGH);
  delayMicroseconds(50);
}

void clearSCL()
{
  digitalWrite(_SCL, LOW);
  delayMicroseconds(50);
}

void setWriteMode()
{
  putData((SLAVE_ADDRESS << 1) & 0x00);
}

void setReadMode()
{
  putData((SLAVE_ADDRESS << 1) & 0x01);
}

uint8_t getACK()
{
  uint8_t ACK;

  pinMode(_SDA, INPUT);
  setSCL();

  ACK = digitalRead(_SDA);

  pinMode(_SDA, OUTPUT);
  clearSCL();
  
  Serial.print("ACK: ");
  Serial.println(ACK);

  return ACK;
}

void putData(uint8_t data)
{
  for(int i = 7; i >= 0; i--)
  {
    digitalWrite(_SDA, (data >> i) & 0x01);

    setSCL();
    clearSCL();
  }

  getACK();
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
  write(0x45);
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