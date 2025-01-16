#pragma once

#include <Arduino.h>

const uint8_t LeftDigit[10][2] = {
{B11110000, B00100010}, // 0
{B00010000, B00100000}, // 1
{B11101000, B00100000}, // 2
{B01111000, B00100000}, // 3
{B00011000, B00100010}, // 4
{B01111000, B00000010}, // 5
{B11101000, B00000010}, // 6
{B00110000, B00100000}, // 7
{B11111000, B00100010}, // 8
{B01111100, B11111111}  // 9
};

const uint8_t RightDigit[10][2] = {
{B00000100, B11010101},
{B00000000, B01010000},
{B00000100, B11001001},
{B00000000, B11011001},
{B00000000, B01011100},
{B00000000, B10011101},
{B00000100, B10011101},
{B00000000, B11010000},
{B00000100, B11011101},
{B00000000, B11011101}
};

const uint8_t LeftLetters[][2] = {
{B00000000, B00000001}, // Underscore
{B00000100, B10000101}, // C
{B00000100, B10001101}  // E
};

const uint32_t BaudRate = 115200;
const uint16_t NetworkPort = 80;

const uint8_t PulseWidth = 5; // Delay in microseconds for proper registers functioning

const uint8_t OutDataPin = 19; // 74hc595 registers
const uint8_t OutClockPin = 26; // 74hc595 registers clock pin
const uint8_t OutLatchPin = 25; // 74hc595 registers latch pin
const uint8_t OutRegistersCount = 5; // 74hc595 registers count

const uint8_t InDataPin = 27; // 74hc165 registers
const uint8_t InClockPin = 4; // 74hc165 registers clock pin
const uint8_t InLatchPin = 13; // 74hc165 registers latch pin 
const uint8_t InRegistersCount = 7; // 74hc165 registers count

const uint8_t BatteryVoltmeterPwmPin = 21; // Vmeter on 1st block

const uint8_t SupplyVoltmeterPwmPin = 22; // kVmeter
const uint8_t EnginesCurrentPositivePwmPin = 23; // Ampermeter positive
const uint8_t EnginesCurrentNegativePwmPin = 15; // Ampereter negative

const uint8_t TmPwmPin = 5; // TM servo pwm pin
const uint8_t NmPwmPin = 17; // NM servo pwm pin
const uint8_t TcPwmPin = 16; // TC servo pwm pin

const uint8_t LightingDataPin1 = 18; // Pult lighting first group
const uint8_t LightingLedCount1 = 8; // LED count for first group

const uint8_t LightingDataPin2 = 12; // Pult lighting second group
const uint8_t LightingLedCount2 = 8; // LED count for second group

const uint8_t GaugesLightingDataPin = 14; // High-voltage gauges lighting
const uint8_t GaugesLightingLedCount = 1; // LED count for gauges

const uint8_t CranePin = 32; // Driver's valve pin

const uint8_t PotentiometerPin1 = 34; // Lighting potentiometer on 3rd block
const uint8_t PotentiometerPin2 = 35; // Lighting potentiometer on 1st block

const uint8_t LightingColorHue = 20;
const uint8_t LightingColorSat = 200; 
const uint8_t LightingColorvalue = 255;