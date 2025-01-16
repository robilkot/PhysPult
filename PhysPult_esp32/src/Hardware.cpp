#include "Hardware.h"

void InitializeHardware(PhysPult& physPult)
{
  pinMode(InDataPin, INPUT);
  pinMode(InLatchPin, OUTPUT);
  pinMode(InClockPin, OUTPUT);

  pinMode(OutDataPin, OUTPUT);
  pinMode(OutLatchPin, OUTPUT);
  pinMode(OutClockPin, OUTPUT);

  pinMode(BatteryVoltmeterPwmPin, OUTPUT);
  pinMode(SupplyVoltmeterPwmPin, OUTPUT);
  pinMode(EnginesCurrentPositivePwmPin, OUTPUT);
  pinMode(EnginesCurrentNegativePwmPin, OUTPUT);

  pinMode(LightingDataPin1, OUTPUT);
  pinMode(LightingDataPin2, OUTPUT);
  pinMode(GaugesLightingDataPin, OUTPUT);
  pinMode(PotentiometerPin1, INPUT);
  pinMode(PotentiometerPin2, INPUT);
  pinMode(CranePin, INPUT);

  pinMode(TmPwmPin, OUTPUT);
  pinMode(TmPwmPin, OUTPUT);
  pinMode(TcPwmPin, OUTPUT);

  FastLED.addLeds<WS2812B, LightingDataPin1, GRB>(physPult.LightingLeds1, sizeof(physPult.LightingLeds1) / sizeof(CRGB));
  // todo: Screw this bs. Crash
  // FastLED.addLeds<WS2812B, LightingDataPin2, GRB>(physPult.LightingLeds2, sizeof(physPult.LightingLeds2) / sizeof(CRGB));
  // FastLED.addLeds<WS2812B, GaugesLightingDataPin, GRB>(physPult.GaugesLeds, sizeof(physPult.GaugesLeds) / sizeof(CRGB));
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);

  // todo: document + move pwm channels to constants.h
  ledcSetup(12, 5000, 8);
  ledcSetup(13, 5000, 8);
  ledcSetup(14, 5000, 8);
  ledcSetup(15, 5000, 8);
  ledcAttachPin(EnginesCurrentNegativePwmPin, 14);
  ledcAttachPin(EnginesCurrentPositivePwmPin, 15);
  ledcAttachPin(SupplyVoltmeterPwmPin, 12);
  ledcAttachPin(BatteryVoltmeterPwmPin, 13);

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
    secondDigitByte = LeftDigit[speed / 10][0] | RightDigit[speed % 10][0] | (physPult.OutRegisters[1] & B00000011);
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
  // delayMicroseconds(PulseWidth);
  digitalWrite(OutLatchPin, 1);

  delay(100);

  UpdateLeds(physPult);
  UpdateServos(physPult);
  UpdateAnalogControls(physPult);
}

void UpdateAnalogControls(PhysPult& physPult)
{
  static TimerMs analogUpdateTimer(5, true, false);

  if(analogUpdateTimer.tick())
  {
    // Battery voltmeter
    ledcWrite(13, physPult.BatteryVoltage);

    // Supply voltmeter
    ledcWrite(12, physPult.SupplyVoltage);

    // Engines ampmeter
    uint8_t enginesCurrentAbsolute = abs(physPult.EnginesCurrent);
    
    if(physPult.EnginesCurrent > 0)
    {
      ledcWrite(14, 0);
      ledcWrite(15, enginesCurrentAbsolute);
    }
    else
    {
      ledcWrite(14, enginesCurrentAbsolute);
      ledcWrite(15, 0);
    }
  }
}

void UpdateServos(PhysPult& physPult)
{
  static uint8_t tmCurrentValue = 0;
  static uint8_t nmCurrentValue = 0;
  static uint8_t tcCurrentValue = 0;
  
  // To prevent too fast servos movements
  static TimerMs servoUpdateTimer(5, true, false);

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
  static bool LightingEnabled = false;

  // Global switch for lighting (both pult + gauges)
  if(physPult.InRegisters[5] >> 3 & 1) 
  {
    LightingEnabled = true;
  }
  else
  {
    LightingEnabled = false; 
  }

  // 1st block
  for(uint8_t i = 0; i < LightingLedCount1; i++)
  {
    // physPult.LightingLeds1[i] = CHSV(LightingColorHue, LightingColorSat, physPult.LightingBrightness1 * LightingEnabled);
    physPult.LightingLeds1[i] = CHSV(LightingColorHue, LightingColorSat, 255);
  }
  // 3rd block
  for(uint8_t i = 0; i < LightingLedCount2; i++)
  {
    // physPult.LightingLeds2[i] = CHSV(LightingColorHue, LightingColorSat, physPult.LightingBrightness2 * LightingEnabled);
    physPult.LightingLeds2[i] = CHSV(LightingColorHue, LightingColorSat, 255);
  }
  // Gauges
  for(uint8_t i = 0; i < GaugesLightingLedCount; i++)
  {
    // physPult.GaugesLeds[i] = CHSV(LightingColorHue, LightingColorSat, 255 * LightingEnabled);
    physPult.GaugesLeds[i] = CHSV(LightingColorHue, LightingColorSat, 255);
  }

  FastLED.show();
}

void UpdateInput(PhysPult& physPult)
{
  // todo: move to StableRead function
  static uint8_t currentMeasurement = 0;

  static uint8_t potentiometerValue1 = 0;
  static uint8_t potentiometerValue2 = 0;
  static uint8_t cranePotentiometerValue = 0;
 
  static uint32_t potentiometerTempValue1 = 0;
  static uint32_t potentiometerTempValue2 = 0;
  static uint32_t cranePotentiometerTempValue = 0;

  // Prevents from flickering since measurements are not single. Doesn't block execution performing multiple analogReads per call.
  if(currentMeasurement < 15)
  {
    potentiometerTempValue1 += analogRead(PotentiometerPin1);
    // potentiometerTempValue2 += analogRead(PotentiometerPin2);
    cranePotentiometerTempValue += analogRead(CranePin);
  
    currentMeasurement++;
  }
  else
  {
    potentiometerTempValue1 /= (currentMeasurement + 1);
    potentiometerTempValue2 /= (currentMeasurement + 1);
    cranePotentiometerTempValue /= (currentMeasurement + 1);

    potentiometerValue1 = map(potentiometerTempValue1, 0, 4096, 255, 0);
    // potentiometerValue2 = map(potentiometerTempValue2 / (currentMeasurement + 1), 0, 4096, 255, 0);
    cranePotentiometerValue = map(cranePotentiometerTempValue, 0, 4096, 0, 255);

    potentiometerTempValue1 = 0;
    // potentiometerTempValue2 = 0;
    cranePotentiometerTempValue = 0;
    currentMeasurement = 0;
  }

  // This check prevents flickering because of inaccurate measurements (even with multiple analogRead)
  auto lightingBrightness1Delta = potentiometerValue1 - physPult.LightingBrightness1;
  // auto lightingBrightness2Delta = potentiometerValue2 - physPult.LightingBrightness2;
  auto cranePositionDelta = cranePotentiometerValue - physPult.CranePosition;
  
  if(abs(lightingBrightness1Delta) > 2)
  {
    physPult.LightingBrightness1 = potentiometerValue1;
    physPult.LightingBrightness2 = potentiometerValue1;
  }
  // if(abs(lightingBrightness2Delta) > 2)
  // {
  //   physPult.LightingBrightness2 = potentiometerValue2;
  // }
  if(abs(cranePositionDelta) > 2)
  {
    physPult.CranePosition = cranePotentiometerValue;
  }

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