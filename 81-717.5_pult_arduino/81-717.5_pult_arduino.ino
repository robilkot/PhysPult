/*
11 latch ST_CP вход 74HC595
12 clock SH_CP вход 74HC595
10 data DS вход 74HC595
*/
constexpr short totalRegisters = 8; // Общее число подключенных регистров

void setup()
{
  DDRB |= B00011100; // set 10, 11, 12 to output
  Serial.begin(9600);
  //registersWrite("1111111100000000010101010000001000000001000000010000000111111111"); // test string
}

void loop()
{
  while(1) if (Serial.available()) registersWrite(Serial.readString());
}

void registersWrite(String toSend)
{   
  PORTB &= ~B00001000; // latch LOW

  for(short currentRegisterNumber = 0; currentRegisterNumber < totalRegisters; currentRegisterNumber++) {
    byte currentRegisterByte = 0;
      for(uint8_t k = 0; k < 8; k++)
        currentRegisterByte |= toSend[currentRegisterNumber*8 + k] - '0' << k;

      for (uint8_t k = 0; k < 8; k++, currentRegisterByte <<= 1)  {
        currentRegisterByte & 128 ? PORTB |= 1 << 2 : PORTB &= ~(1 << 2);
        PORTB |= B00010000; // clock HIGH
        PORTB &= ~B00010000; // clock LOW
      }
  }

  PORTB |= B00001000; // latch HIGH
}