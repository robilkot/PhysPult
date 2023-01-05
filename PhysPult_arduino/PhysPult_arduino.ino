void setup()
{
  //DDRB |= B11111111; // set 8-13 to output
  //DDRD |= B11111110; // set 1-7 to output
  Serial.begin(115200);
  Serial.setTimeout(5);
}

void loop()
{
  /*while(Serial.available() > 0 && Serial.peek() != '{') char t = Serial.read();
  String command = "";

  Serial.read();
  while(true) {
    if(Serial.available() > 0){
      char temp = Serial.read();
      if(temp=='}') break;
      command+=temp;
    }
  }
	
  Serial.print(command+"}");
*/

/*if(command == "PhysPultInit") {
    Serial.print("{PhysPultInitOK}");
  } else */
  
  //registersWrite(command, 8, 6, 9, 10);
  //Serial.print("{" + registersRead(1, 8, 10, 9) + "}");

String state = "{";
for(int i = 0; i<64; i++) {
  randomSeed(analogRead(0));
	state+=random(0,2);
}
state+="}";
Serial.println(state);

delay(33);
}

void registersWrite(String toSend, uint8_t totalRegisters, uint8_t data, uint8_t latch, uint8_t clock)
{   
  volatile uint8_t *DADDR = data > 7 ? &PORTB : &PORTD,
                   *LADDR = latch > 7 ? &PORTB : &PORTD,
                   *CADDR = clock > 7 ? &PORTB : &PORTD;

  bool Dshift = DADDR == &PORTB,
       Lshift = LADDR == &PORTB,
       Cshift = CADDR == &PORTB;

  *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW

  for (uint8_t i = 0; i < 8 * totalRegisters; i++)  {
    toSend[i] - '0' ? *DADDR |= 1 << data - 8 * Dshift : *DADDR &= ~(1 << data - 8 * Dshift); // write 1 or 0 to corresponding bit
    *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
    *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
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