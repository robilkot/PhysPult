byte leftdigit[10][2] = {
{B00010000, B11101001},
{B00010000, B00001000},
{B00010000, B10110001},
{B00010000, B10111000},
{B00010000, B01011000},
{B00000000, B11111000},
{B00000000, B11111001},
{B00010000, B10001000},
{B00010000, B11111001},
{B00010000, B11111000}
};

byte rightdigit[10][2] = {
{B11100010, B00000110},
{B11000000, B00000000},
{B10100001, B00000110},
{B11100001, B00000010},
{B11000011, B00000000},
{B01100011, B00000010},
{B01100011, B00000110},
{B11100000, B00000000},
{B11100011, B00000110},
{B11100011, B00000010}
};

#define BaudRate 9600

void setup()
{
  DDRB |= B11111111; // set 8-13 to output
  DDRD |= B11111110; // set 1-7 to output

  Serial.begin(BaudRate);
  Serial.setTimeout(5);
  
  digitalWrite(6, 1);

  byte on[5] = {88, 255, 255, 255, 255},
      off[5] = {0, 0, 0, 0, 0};

  updateIndicators(on, 4, 5, 6);
  delay(500);
  updateIndicators(off, 4, 5, 6);
  delay(100);
}

void loop()
{

  if(Serial.available()) {
    byte command[12];

    for(int i = 0; i < 12; i++) command[i] = 0;

    // String temp = Serial.readStringUntil(';');
    // command[temp[0]-'0'] = atoi(temp.substring(1,temp.length()).c_str());
    // Serial.print(command[temp[0]-'0'], DEC);
    // Serial.print('\n');
    
    Serial.readBytesUntil('}', command, 64);
    
    updateIndicators(command, 4, 5, 6);
  }
}

void updateIndicators(byte* command, uint8_t data, uint8_t clock, uint8_t latch)
{
  volatile uint8_t *DADDR = data > 7 ? &PORTB : &PORTD,
                   *LADDR = latch > 7 ? &PORTB : &PORTD,
                   *CADDR = clock > 7 ? &PORTB : &PORTD;

  bool Dshift = DADDR == &PORTB,
       Lshift = LADDR == &PORTB,
       Cshift = CADDR == &PORTB;

  char speed = command[0];
  byte secondSpeedByte = leftdigit[speed / 10][0] | rightdigit[speed % 10][0] | command[1];
  byte firstSpeedByte = leftdigit[speed / 10][1] | rightdigit[speed % 10][1]; 

  *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW

  shiftOut(4, 5, LSBFIRST, command[2]);
  shiftOut(4, 5, LSBFIRST, command[3]);

  shiftOut(4, 5, LSBFIRST, secondSpeedByte); // Регистр 2
  shiftOut(4, 5, LSBFIRST, firstSpeedByte); // Регистр 1 

  *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
}

// void registersWrite(String toSend, uint8_t data, uint8_t latch, uint8_t clock)
// {   
//   volatile uint8_t *DADDR = data > 7 ? &PORTB : &PORTD,
//                    *LADDR = latch > 7 ? &PORTB : &PORTD,
//                    *CADDR = clock > 7 ? &PORTB : &PORTD;

//   bool Dshift = DADDR == &PORTB,
//        Lshift = LADDR == &PORTB,
//        Cshift = CADDR == &PORTB;

//   *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW

//   for (uint8_t i = 0; i < toSend.length(); i++)  {
//     for(int k = 0; k < 8; k++, toSend[i] >>= k) {
//       (toSend[i] & 1) ? *DADDR |= 1 << data - 8 * Dshift : *DADDR &= ~(1 << data - 8 * Dshift); // write 1 or 0 to corresponding bit
//       *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
//       *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
//     }
//   }

//   *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
// }

// String registersRead(uint8_t totalRegisters, uint8_t data, uint8_t latch, uint8_t clock)
// {
//   volatile uint8_t *DADDR = data > 7 ? &PINB : &PIND,
//                    *LADDR = latch > 7 ? &PORTB : &PORTD,
//                    *CADDR = clock > 7 ? &PORTB : &PORTD;

//   bool Dshift = DADDR == &PINB,
//        Lshift = LADDR == &PORTB,
//        Cshift = CADDR == &PORTB;

//   String ret;

//   *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW
//   *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH

//   for (uint8_t i = 0; i < totalRegisters * 8; i++) {
//     ret += *DADDR & (1 << data - 8 * Dshift) == 1 << data - 8 * Dshift;
  
//     *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
//     *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
//   }
  
//   return ret;
// }