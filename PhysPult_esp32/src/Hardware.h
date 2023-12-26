#include <Arduino.h>
#include <Constants.h>

#pragma once

void WriteVoltmeter(uint8_t voltage);
void WriteOutRegisters(uint8_t* command);
uint8_t ReadInRegister(uint8_t data, uint8_t clock);
void ReadInRegisters(byte* const output);
void SetIndicatorsOn();
void SetIndicatorsOff();