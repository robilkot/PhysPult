#include <Arduino.h>

#pragma once

const uint8_t LeftDigit[10][2] = {
{B00010000, B11101001},
{B00010000, B00001000},
{B00010000, B10110001},
{B00010000, B10111000},
{B00010000, B01011000},
{B00000000, B11111000},
{B00000000, B11111001},
{B00010000, B10001000},
{B00010000, B11111001},
{B00010000, B11111000}
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

const uint32_t BaudRate = 115200;
const uint16_t NetworkPort = 80;

// const uint16_t idlemode_delay = 1050; // Delay before going into idle mode
// const uint16_t SwitchesUpdateInterval = 30; // Interval for updating switches state

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
const uint8_t EnginesCurrentPwmPin = 23; // Ameter

const uint8_t TmPwmPin = 5;
const uint8_t NmPwmPin = 17;
const uint8_t TcPwmPin = 16;

const uint8_t LightingDataPin = 18; // Pult lighting
const uint8_t LightingLedCount = 8;

const uint8_t PotentiometerOnePin = 34; // Lighting potentiometer on 1st block
const uint8_t PotentiometerTwoPin = 35; // Lighting potentiometer on 3rd block

// const uint8_t LightingColorHue = 25; // For MicroLED
// const uint8_t LightingColorSat = 200; 
// const uint8_t LightingColorvalue = 50; 
const uint8_t LightingColorHue = 30; // For FastLED
const uint8_t LightingColorSat = 200; 
const uint8_t LightingColorvalue = 150; 