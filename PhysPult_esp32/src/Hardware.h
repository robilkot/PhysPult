#include <Arduino.h>
#include "Constants.h"
#include "PhysPult.h"

#pragma once

void WriteVoltmeter(uint8_t voltage);
void WriteOutRegisters(uint8_t* command);
void ReadInRegisters(uint8_t* output);
void BackgroundHardwareFunction(void*);