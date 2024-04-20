#pragma once

#include <Arduino.h>

const uint8_t LeftDigit[10][2] = {
{B00010000, B11101001}, // 0
{B00010000, B00001000}, // 1
{B00010000, B10110001}, // 2
{B00010000, B10111000}, // 3
{B00010000, B01011000}, // 4
{B00000000, B11111000}, // 5
{B00000000, B11111001}, // 6
{B00010000, B10001000}, // 7
{B00010000, B11111001}, // 8
{B00010000, B11111000}  // 9
};

const uint8_t RightDigit[10][2] = {
{B11100010, B00000110},
{B11000000, B00000000},
{B10100001, B00000110},
{B11100001, B00000010},
{B11000011, B00000000},
{B01100011, B00000010},
{B01100011, B00000110},
{B11100000, B00000000},
{B11100011, B00000110},
{B11100011, B00000010}
};

const uint8_t LeftLetters[][2] = {
{B00000000, B00100000}, // Underscore
{B00000000, B11100001}, // C
{B00000000, B11110001}  // E
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
const uint8_t InRegistersCount = 6; // 74hc165 registers count

const uint8_t BatteryVoltmeterPwmPin = 21; // Vmeter on 1st block

const uint8_t SupplyVoltmeterPwmPin = 22; // kVmeter
const uint8_t EnginesCurrentPositivePwmPin = 23; // Ampermeter positive
const uint8_t EnginesCurrentNegativePwmPin = 15; // Ampereter negative

const uint8_t TmPwmPin = 5; // TM servo pwm pin
const uint8_t NmPwmPin = 17; // NM servo pwm pin
const uint8_t TcPwmPin = 16; // TC servo pwm pin

const uint8_t LightingDataPin1 = 18; // Pult lighting first group
const uint8_t LightingLedCount1 = 4; // LED count for first group

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