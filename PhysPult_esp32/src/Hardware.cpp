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
  delayMicroseconds(PulseWidth);

  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[8]); // Register 5
  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[6]); 
  // shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[7]); 
  for(uint8_t i = 1; i < OutRegistersCount; i++)
  {
    shiftOut(OutDataPin, OutClockPin, LSBFIRST, command[i]); 
  }

  shiftOut(OutDataPin, OutClockPin, LSBFIRST, secondSpeedByte);
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, firstSpeedByte); // Register 1 

  delayMicroseconds(PulseWidth);
  digitalWrite(OutLatchPin, 1);
}

void ReadInRegisters(uint8_t* output)
{
  digitalWrite(InLatchPin, LOW);    
  delayMicroseconds(PulseWidth);
  digitalWrite(InLatchPin, HIGH);

  for(uint8_t i = 0; i < InRegistersCount; i++)
  {
    for(uint8_t k = 0; k < 8; k++)
    {
      if(digitalRead(InDataPin) == 1)
      {
        bitSet(output[i], k);
      }
      else
      {
        bitClear(output[i], k);
      }

      digitalWrite(InClockPin, HIGH);
      delayMicroseconds(PulseWidth);
      digitalWrite(InClockPin, LOW);
    }
  }    
}

// This function must be an infinite loop or terminate itself at the end with vTaskDelete();
void BackgroundHardwareFunction(void *pvParameters)
{
  PhysPult& physPult = *((PhysPult*)pvParameters);
  bool flag = 0;
  uint8_t cnt = 0;

  while(true)
  {
    delay(50);

    ReadInRegisters(physPult.Input);

    fill_rainbow(physPult.LightingLeds, LightingLedCount, cnt++);
    FastLED.show();

    uint8_t command[] = {11, 0, 0, 0, 0};
    WriteOutRegisters(command);
      

    flag = !flag;
    if(flag) {
    
    }


    for(uint8_t i = 0; i < InRegistersCount; i++)
    {
      for(uint8_t k = 0; k < 8; k++)
      {
        Serial.print(physPult.Input[i] >> k & 1);
      }
      Serial.print(' ');
    }
    Serial.println();
  }
}