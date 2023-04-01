byte leftdigit[10][2] = {
{B00010000, B00101111},
{B00010000, B00100000},
{B00010000, B00011011},
{B00010000, B00111010},
{B00010000, B00110100},
{B00000000, B00111110},
{B00000000, B00111111},
{B00010000, B00100010},
{B00010000, B00111111},
{B00010000, B00111110}
};

byte rightdigit[10][2] = {
{B10001110, B11000000},
{B00000110, B00000000},
{B00001011, B11000000},
{B00001111, B10000000},
{B10000111, B00000000},
{B10001101, B10000000},
{B10001101, B11000000},
{B00001110, B00000000},
{B10001111, B11000000},
{B10001111, B10000000}
};

void setup()
{
  DDRB |= B11111111; // set 8-13 to output
  DDRD |= B11111110; // set 1-7 to output

  Serial.begin(57600);
  Serial.setTimeout(5);
  
  digitalWrite(6, 1);
}

//AsyncStream<200> serial(&Serial, '}', 15); // Инициализация асинхронного считывателя

void loop()
{
  if (Serial.available()) {     // Если данные получены
  //char* command = serial.buf + 1; // Обрезка '{'
  //String command = Serial.readString();
  //Serial.print('{' + command + '}');

  byte command[9];
  Serial.readBytes(command, 9);

  byte firstSpeedByte = leftdigit[(int)command[1] / 10][1] | rightdigit[(int)command[1] % 10][1]; 
  byte secondSpeedByte = leftdigit[(int)command[1] / 10][0] | rightdigit[(int)command[1] % 10][0];

  digitalWrite(6, 0);

  for(int i = 2; i < 9-1; i++) { // Индексациия с 1 т.к. первый байт - скорость
    //SPI.transfer(command[i]);
    shiftOut(4, 5, MSBFIRST, command[i]); // Пишем на регистры значения
  }

  shiftOut(4, 5, MSBFIRST, secondSpeedByte); // Записываем скорость
  shiftOut(4, 5, MSBFIRST, firstSpeedByte); 

  digitalWrite(6, 1);

  //registersWrite(command.substring(2, command.length() - 2) + secondSpeedByte + firstSpeedByte, 4, 6, 5);
  //delay(100);
  while(Serial.available())
    Serial.read();
  }
}

void registersWrite(String toSend, uint8_t data, uint8_t latch, uint8_t clock)
{   
  volatile uint8_t *DADDR = data > 7 ? &PORTB : &PORTD,
                   *LADDR = latch > 7 ? &PORTB : &PORTD,
                   *CADDR = clock > 7 ? &PORTB : &PORTD;

  bool Dshift = DADDR == &PORTB,
       Lshift = LADDR == &PORTB,
       Cshift = CADDR == &PORTB;

  *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW

  for (uint8_t i = 0; i < toSend.length(); i++)  {
    for(int k = 0; k < 8; k++, toSend[i] >>= k) {
      (toSend[i] & 1) ? *DADDR |= 1 << data - 8 * Dshift : *DADDR &= ~(1 << data - 8 * Dshift); // write 1 or 0 to corresponding bit
      *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
      *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
    }
  }

  *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
}

String registersRead(uint8_t totalRegisters, uint8_t data, uint8_t latch, uint8_t clock)
{
  volatile uint8_t *DADDR = data > 7 ? &PINB : &PIND,
                   *LADDR = latch > 7 ? &PORTB : &PORTD,
                   *CADDR = clock > 7 ? &PORTB : &PORTD;

  bool Dshift = DADDR == &PINB,
       Lshift = LADDR == &PORTB,
       Cshift = CADDR == &PORTB;

  String ret;

  *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW
  *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH

  for (uint8_t i = 0; i < totalRegisters * 8; i++) {
    ret += *DADDR & (1 << data - 8 * Dshift) == 1 << data - 8 * Dshift;
  
    *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
    *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
  }
  
  return ret;
}