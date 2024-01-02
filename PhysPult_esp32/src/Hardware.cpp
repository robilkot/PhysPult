#include "Hardware.h"

void InitializeHardware(PhysPult& physPult)
{
  Serial.begin(BaudRate);

  pinMode(InDataPin, INPUT);
  pinMode(InLatchPin, OUTPUT);
  pinMode(InClockPin, OUTPUT);

  pinMode(OutDataPin, OUTPUT);
  pinMode(OutLatchPin, OUTPUT);
  pinMode(OutClockPin, OUTPUT);

  pinMode(BatteryVoltmeterPwmPin, OUTPUT);
  pinMode(SupplyVoltmeterPwmPin, OUTPUT);
  pinMode(EnginesCurrentPwmPin, OUTPUT);

  pinMode(LightingDataPin, OUTPUT);
  pinMode(PotentiometerPin1, INPUT);
  // pinMode(PotentiometerPin2, INPUT);

  pinMode(TmPwmPin, OUTPUT);
  pinMode(TmPwmPin, OUTPUT);
  pinMode(TcPwmPin, OUTPUT);

  FastLED.addLeds<WS2812B, LightingDataPin, GRB>(physPult.LightingLeds, LightingLedCountTotal);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);

  physPult.TmServo.attach(TmPwmPin); 
  physPult.NmServo.attach(NmPwmPin); 
  physPult.TcServo.attach(TcPwmPin); 
}

void AdjustValue(uint8_t& currentValue, const uint8_t targetValue)
{
  if(currentValue > targetValue)
  {
    currentValue--;
  }
  else if(currentValue < targetValue)
  {
    currentValue++;
  }
}

void DisplayState(PhysPult& physPult)
{
  // To blink output and not let user think that program is dead
  static bool blinkNow = false;
  static TimerMs blinkTimer(350, true, true);

  if(blinkTimer.elapsed())
  {
    // Set difference for lit and unlit periods of time if needed
    // blinkTimer.setTime(blinkNow ? 750 : 250);  
    blinkTimer.start();

    blinkNow = !blinkNow;
  }

  uint8_t secondDigitByte = 0;
  uint8_t firstDigitByte = 0;

  // Start shifting out
  digitalWrite(OutLatchPin, 0);
  delayMicroseconds(PulseWidth);

  if(physPult.State == NetworkInitialization)
  {
    secondDigitByte = LeftLetters[1][0];
    firstDigitByte = LeftLetters[1][1];
    
    for(uint8_t i = 2; i < OutRegistersCount; i++)
    {
      shiftOut(OutDataPin, OutClockPin, LSBFIRST, 0);  
    }
  }
  else if(physPult.State == WaitingForClient)
  {
    // if(blinkNow == false)
    // {
      uint8_t displayNumber = physPult.DeviceNumber;

      secondDigitByte = LeftDigit[displayNumber / 10][0] | RightDigit[displayNumber % 10][0];
      firstDigitByte = LeftDigit[displayNumber / 10][1] | RightDigit[displayNumber % 10][1]; 
    // }

    for(uint8_t i = 2; i < OutRegistersCount; i++)
    {
      shiftOut(OutDataPin, OutClockPin, LSBFIRST, 0);  
    }
  } 
  else // State == Work
  {
    uint8_t speed = physPult.Speed % 100;

    // LKVC and LSN have been taken into account at the end of next line. 
    secondDigitByte = LeftDigit[speed / 10][0] | RightDigit[speed % 10][0] | (physPult.OutRegisters[1] & B00001100);
    firstDigitByte = LeftDigit[speed / 10][1] | RightDigit[speed % 10][1]; 

    // Starting with 2 because 0 and 1 are registers working with speedometer.
    // Last 2 bits of 2nd register are responcible for LKVC and LSN and have been taken into account above. 
    for(uint8_t i = OutRegistersCount - 1; i >= 2; i--)
    {
      shiftOut(OutDataPin, OutClockPin, LSBFIRST, physPult.OutRegisters[i]);  
    }
  }

  // for(uint8_t i = 0; i < InRegistersCount; i++) {
  //     for(uint8_t k = 0; k < 8; k++) Serial.print(physPult.InRegisters[i] >> (7 - k) & 1);
  //     Serial.print(' ');
  //   }
  //   Serial.println();

  shiftOut(OutDataPin, OutClockPin, LSBFIRST, secondDigitByte);
  shiftOut(OutDataPin, OutClockPin, LSBFIRST, firstDigitByte); 

  // End shifting out
  delayMicroseconds(PulseWidth);
  digitalWrite(OutLatchPin, 1);

  // Battery voltmeter
  analogWrite(BatteryVoltmeterPwmPin, physPult.BatteryVoltage);

  // todo: implement
  // Supply voltmeter
  // analogWrite(SupplyVoltmeterPwmPin, map(physPult.SupplyVoltage, 0, 1000, 0, 255));

  // todo: How to implement negative values?
  // Engines ampmeter
  // analogWrite(EnginesCurrentPwmPin, map(physPult.EnginesCurrent, -500, 500, 0, 255));

  UpdateLeds(physPult);
  UpdateServos(physPult);
}

void UpdateServos(PhysPult& physPult)
{
  // Track current and target values to prevent too fast servos movements
  static uint8_t tmCurrentValue = 0;
  static uint8_t nmCurrentValue = 0;
  static uint8_t tcCurrentValue = 0;
  
  static TimerMs servoUpdateTimer(20, true, false);

  if(servoUpdateTimer.tick())
  {
    AdjustValue(tmCurrentValue, physPult.TmValue);
    AdjustValue(nmCurrentValue, physPult.NmValue);
    AdjustValue(tcCurrentValue, physPult.TcValue);
    
    // Adjustments caused by physical behaviour of servos
    physPult.TmServo.write(map(tmCurrentValue, 0, 255, 0, 180));
    physPult.NmServo.write(255 - map(nmCurrentValue, 0, 255, 80, 255));
    physPult.TcServo.write(tcCurrentValue);
  }
}

void UpdateLeds(PhysPult& physPult)
{

  // Commented out since both blocks are connected to the same potentiometer and data pin
  // 3rd block
  // for(uint8_t i = 0; i < LightingLedCountTotal; i++)
  // {
  //   physPult.LightingLeds[i] = CHSV(LightingColorHue, LightingColorSat, physPult.LightingBrightness1);
  // }
  // // 1st block
  // for(uint8_t i = LightingLedCount1; i < LightingLedCountTotal; i++)
  // {
  //   physPult.LightingLeds[i] = CHSV(LightingColorHue, LightingColorSat, physPult.LightingBrightness2);
  // }

  if(physPult.InRegisters[5] >> 3 & 1) 
  {
    FastLED.setBrightness(physPult.LightingBrightness1);
  }
  else
  {
    FastLED.setBrightness(0);  
  }

  FastLED.show();
}

uint16_t StableRead(uint8_t pin, uint8_t iterations)
{
  uint64_t result = 0;

  for(uint8_t i = 0; i < iterations; i++)
  {
    result += analogRead(pin);
  }

  result /= iterations;

  return result;
}

void UpdateInput(PhysPult& physPult)
{
  physPult.LightingBrightness1 = map(StableRead(PotentiometerPin1, 30), 0, 4096, 255, 0);

  // todo: implement
  // physPult.CranePosition = map(StableRead(CranePin, 20), 0, 4096, 0, 255);

  // Commented out since second potentiometer is not used
  // physPult.LightingBrightness2 = physPult.LightingBrightness1; // map(analogRead(PotentiometerPin2), 0, 4096, 0, 255);

  ReadInRegisters(physPult.InRegisters);
}

void ReadInRegisters(uint8_t* output)
{
  digitalWrite(InLatchPin, LOW);    
  delayMicroseconds(PulseWidth);
  digitalWrite(InLatchPin, HIGH);

  for(uint8_t i = 0; i < InRegistersCount; i++)
  {
    output[i] = shiftIn(InDataPin, InClockPin, LSBFIRST);
  }    
}

// This function must have an infinite loop or terminate itself at the end with vTaskDelete();
void BackgroundHardwareFunction(void *pvParameters)
{
  PhysPult& physPult = *((PhysPult*)pvParameters);
  
  while(true)
  {
    UpdateInput(physPult);
    DisplayState(physPult);
  }
}