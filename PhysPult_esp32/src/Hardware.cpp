#include "Hardware.h"

void WriteVoltmeter(uint8_t voltage)
{
  voltage %= 151; // Overflow protection
  analogWrite(VoltmeterPwmPin, map(voltage, 0, 150, 0, 255));
}

void WriteOutRegisters(uint8_t* command)
{
  uint8_t speed = command[0];
  uint8_t secondSpeedByte = LeftDigit[speed / 10][0] | RightDigit[speed % 10][0] | command[5];
  uint8_t firstSpeedByte = LeftDigit[speed / 10][1] | RightDigit[speed % 10][1]; 

  digitalWrite(OutLatchPin, 0);
  //delayMicroseconds(pulseWidth);

  shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[8]); // Register 5
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[6]); 
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[7]); 

  shiftOut(OutDataPin, OutClockPin, LSBFIRST, secondSpeedByte);
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, firstSpeedByte); // Register 1 

  //delayMicroseconds(pulseWidth);
  digitalWrite(OutLatchPin, 1);
}

uint8_t ReadInRegister(uint8_t data, uint8_t clock)
{
  uint8_t output = 0;

  // The first one that is read is the highest bit (input D7 of the 74HC165).
  for(int i = 7; i >= 0; i--)
  {
    if(digitalRead(data) == HIGH)
      bitSet(output, i);

    digitalWrite(clock, HIGH);
    delayMicroseconds(10);
    digitalWrite(clock, LOW);
  }

  return output;
}

void ReadInRegisters(byte* const output)
{
  digitalWrite(InLatchPin, LOW);    
  delayMicroseconds(10);
  digitalWrite(InLatchPin, HIGH);

  for(uint8_t i = 0; i < InRegistersCount; i++)
  {
    output[i] = ReadInRegister(InDataPin, InClockPin);           
  }
}

void SetIndicatorsOn()
{
  uint8_t on[5] = {88, 255, 255, 255, 255};
  WriteOutRegisters(on);
}

void SetIndicatorsOff()
{
  uint8_t off[5] = {0, 0, 0, 0, 0};
  WriteOutRegisters(off);
}