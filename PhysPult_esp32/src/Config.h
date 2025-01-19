#pragma once

#include <Arduino.h>
#include <chsv.h>

enum class ControllerPosition : uint8_t {
    O, // Neutral
    X1,
    X2,
    X3,
    T1,
    T1A,
    T2
};

enum class ReverserPosition : uint8_t {
    O, // Neutral
    F, // Forward
    R // Rear
};

static uint32_t BaudRate = 115200;
static uint16_t NetworkPort = 8080;

static CHSV LightingColor{20, 200, 255};

static esp_log_level_t LogLevel = ESP_LOG_DEBUG;

const uint8_t OutDataPin = 19; // 74hc595 registers
const uint8_t OutClockPin = 26; // 74hc595 registers clock pin
const uint8_t OutLatchPin = 25; // 74hc595 registers latch pin
const uint8_t OutRegistersCount = 5; // 74hc595 registers count
const uint8_t OutputsCount = OutRegistersCount * 8;

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