#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <ESP32Servo.h>
#include <TimerMs.h>

#include "Constants.h"
#include "States.h"
#include "PhysPult.h"

void InitializeHardware(PhysPult&);
void DisplayState(PhysPult&);
void UpdateInput(PhysPult&);
void UpdateServos(PhysPult&);
void UpdateLeds(PhysPult&);
void UpdateAnalogControls(PhysPult&);
void ReadInRegisters(uint8_t*);
void BackgroundHardwareFunction(void*);