void setup()
{
  DDRB |= B11111111; // set 8-13 to output
  Serial.begin(9600);
  //registersWrite("1111111100000000010101010000001000000001000000010000000111111111", 8, 8, 9, 10);
}

void loop()
{
 if (Serial.available()) registersWrite(Serial.readString(), 8, 8, 9, 10);
  //Serial.println(registersRead(1, 8, 10, 9));
}

void registersWrite(String toSend, uint8_t totalRegisters, uint8_t data, uint8_t latch, uint8_t clock)
{   
  // todo: make support for both PORTB and PORTD
  PORTB &= ~(1 << latch - 8); // latch LOW

  for (uint8_t k = 0; k < 8 * totalRegisters; k++)  {
    toSend[k] - '0' ? PORTB |= (1 << data - 8) : PORTB &= ~(1 << data - 8); // write 1 or 0 to corresponding bit
    PORTB |= (1 << clock - 8); // clock HIGH
    PORTB &= ~(1 << clock - 8); // clock LOW
  }

  PORTB |= (1 << latch - 8); // latch HIGH
}

String registersRead(uint8_t totalRegisters, uint8_t data, uint8_t latch, uint8_t clock) {
  PORTB &= ~(1 << latch - 8); // latch LOW
  PORTB |= (1 << latch - 8); // latch HIGH

  String ret;
  for (int i = 0; i < totalRegisters * 8; i++) {
    ret += (PINB & (1 << data - 8)) == (1 << data - 8 ) ?  "1" : "0";
  
    PORTB |= (1 << clock - 8); // clock HIGH
    PORTB &= ~(1 << clock - 8); // clock LOW
  }
  return ret;
}