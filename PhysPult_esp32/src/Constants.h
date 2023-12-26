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
const uint16_t SwitchesUpdateInterval = 30; // Interval for updating switches state

const uint8_t OutDataPin = 2; // 74hc595 registers
const uint8_t OutClockPin = 4; // 74hc595 registers clock pin
const uint8_t OutLatchPin = 7; // 74hc595 registers latch pin
const uint8_t OutRegistersCount = 5; // 74hc595 registers count

const uint8_t InDataPin = 8; // 74hc165 registers
const uint8_t InClockPin = 12; // 74hc165 registers clock pin
const uint8_t InLatchPin = 13; // 74hc165 registers latch pin 
const uint8_t InRegistersCount = 6; // 74hc165 registers count

const uint8_t VoltmeterPwmPin = 5;

const uint8_t TmPwmPin = 9;
const uint8_t NmPwmPin = 10;
const uint8_t TcPwmPin = 11;

const uint8_t LightingDataPin = 3; // Pult lighting
const uint8_t LightingLedCount = 8;

// const uint8_t LightingColorHue = 25; // For MicroLED
// const uint8_t LightingColorSat = 200; 
// const uint8_t LightingColorvalue = 50; 
const uint8_t LightingColorHue = 30; // For FastLED
const uint8_t LightingColorSat = 200; 
const uint8_t LightingColorvalue = 150; 