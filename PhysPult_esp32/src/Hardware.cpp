#include "Hardware.h"

void WriteVoltmeter(uint8_t voltage)
{
  voltage %= 151; // Overflow protection
  analogWrite(BatteryVoltmeterPwmPin, map(voltage, 0, 150, 0, 255));
}

void WriteOutRegisters(uint8_t* command)
{
  uint8_t speed = command[0];
  uint8_t secondSpeedByte = LeftDigit[speed / 10][0] | RightDigit[speed % 10][0] ;//| command[5];
  uint8_t firstSpeedByte = LeftDigit[speed / 10][1] | RightDigit[speed % 10][1]; 

  digitalWrite(OutLatchPin, 0);
  delayMicroseconds(20);

  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[8]); // Register 5
  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[6]); 
  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[7]); 

  shiftOut(OutDataPin, OutClockPin, LSBFIRST, secondSpeedByte);
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, firstSpeedByte); // Register 1 

  delayMicroseconds(20);
  digitalWrite(OutLatchPin, 1);
}

void ReadInRegisters(uint8_t* output)
{
  digitalWrite(InLatchPin, 0);    
  delayMicroseconds(50);
  digitalWrite(InLatchPin, 1);
  delayMicroseconds(50);

  for(uint8_t j = 0; j < InRegistersCount; j++)
  {    
    output[j] = shiftIn(InDataPin, InClockPin, LSBFIRST);
  }
}

void SetIndicatorsOn()
{
  uint8_t on[OutRegistersCount] = {88, 255, 255, 255, 255};
  WriteOutRegisters(on);
}

void SetIndicatorsOff()
{
  uint8_t off[OutRegistersCount] = {0, 0, 0, 0, 0};
  WriteOutRegisters(off);
}

// This function must be an infinite loop or terminate itself at the end with vTaskDelete();
void BackgroundHardwareFunction(void *pvParameters)
{
  PhysPult& physPult = *((PhysPult*)pvParameters);
  bool flag = 0;

  BackgroundHardwareRepeat:

  delay(10);
  ReadInRegisters(physPult.Input);

  // flag = !flag;
  // if(flag) {
  //   SetIndicatorsOn();
  // } else {
  //   SetIndicatorsOff();
  // }

  for(uint8_t i = 0; i < InRegistersCount; i++)
  {
    for(uint8_t k = 0; k < 8; k++)
    {
      Serial.print(physPult.Input[i] >> k & 1);
    }
    Serial.print(' ');
  }
  Serial.println();

  goto BackgroundHardwareRepeat;
}