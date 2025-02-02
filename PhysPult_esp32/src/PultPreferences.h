#pragma once

#include <Arduino.h>
#include <chsv.h>
#include <Preferences.h>


// Values that may be changed in runtime
volatile static uint32_t SerialBaudRate = 115200;
volatile static uint16_t NetworkPort = 8080;

volatile static char* NetworkSsid = nullptr; // todo
volatile static char* NetworkPass = nullptr; // todo

volatile static bool SyncController;
volatile static bool SyncReverser;
volatile static bool SyncCrane;
volatile static bool SyncInputRegisters;
volatile static bool EnableGaugesLighting;
volatile static bool EnablePotentiometer;

volatile static CHSV PultLightingColor{29,190,255};
volatile static CHSV GaugesLightingColor{29,190,255};

volatile static esp_log_level_t LogLevel = ESP_LOG_DEBUG;


enum class PultPreferencesKeys {
    SyncController,
    SyncReverser,
    SyncCrane,
    SyncInputRegisters,

    EnablePotentiometer,
    EnableGaugesLighting,

    PultLightingH,
    PultLightingS,
    PultLightingV,
    GaugesLightingH,
    GaugesLightingS,
    GaugesLightingV,

    NetworkSsid,
    NetworkPass,
    NetworkPort,

    SerialBaudRate,
    LogLevel,
};

union PultPreferenceValue {
    bool boolean;
    int number;
    char* string; // todo
};

static void set_pult_preference(PultPreferencesKeys key, const PultPreferenceValue& value)
{
    switch (key)
    {
        // string preferences TODO
    // case PultPreferencesKeys::NetworkSsid: {
    //     NetworkSsid = value.string;
    //     break;
    // }
    // case PultPreferencesKeys::NetworkPass: {
    //     NetworkPass = value.string;
    //     break;
    // }
        // integer preferences
    case PultPreferencesKeys::NetworkPort: {
        NetworkPort = value.number;
        break;
    }
    case PultPreferencesKeys::SerialBaudRate: {
        SerialBaudRate = value.number;
        break;
    }

    case PultPreferencesKeys::PultLightingH: {
        PultLightingColor.h = value.number;
        break;
    }
    case PultPreferencesKeys::PultLightingS: {
        PultLightingColor.s = value.number;
        break;
    }
    case PultPreferencesKeys::PultLightingV: {
        PultLightingColor.v = value.number;
        break;
    }

    case PultPreferencesKeys::GaugesLightingH: {
        GaugesLightingColor.h = value.number;
        break;
    }
    case PultPreferencesKeys::GaugesLightingS: {
        GaugesLightingColor.s = value.number;
        break;
    }
    case PultPreferencesKeys::GaugesLightingV: {
        GaugesLightingColor.v = value.number;
        break;
    }
        // bool preferences
    case PultPreferencesKeys::SyncController: {
        SyncController = value.boolean;
        break;
    }
    case PultPreferencesKeys::SyncReverser: {
        SyncReverser = value.boolean;
        break;
    }
    case PultPreferencesKeys::SyncCrane: {
        SyncCrane = value.boolean;
        break;
    }
    case PultPreferencesKeys::SyncInputRegisters: {
        SyncInputRegisters = value.boolean;
        break;
    }

    case PultPreferencesKeys::EnablePotentiometer: {
        EnablePotentiometer = value.boolean;
        break;
    }
    case PultPreferencesKeys::EnableGaugesLighting: {
        EnableGaugesLighting = value.boolean;
        break;
    }
    default:
        log_e("invalid preference key %d", key);
        break;
    }

    log_i("preference %d set to %d", key, value);
}

// Saves preferences to NVS from PultPreferences namespace variables
static void save_pult_preferences()
{
    Preferences preferences;
    preferences.begin("PhysPult", false);

    preferences.putBool(std::to_string((int)PultPreferencesKeys::EnableGaugesLighting).c_str(), EnableGaugesLighting);
    preferences.putBool(std::to_string((int)PultPreferencesKeys::EnablePotentiometer).c_str(), EnablePotentiometer);

    preferences.putBool(std::to_string((int)PultPreferencesKeys::SyncController).c_str(), SyncController);
    preferences.putBool(std::to_string((int)PultPreferencesKeys::SyncReverser).c_str(), SyncReverser);
    preferences.putBool(std::to_string((int)PultPreferencesKeys::SyncCrane).c_str(), SyncCrane);
    preferences.putBool(std::to_string((int)PultPreferencesKeys::SyncInputRegisters).c_str(), SyncInputRegisters);
    
    preferences.putUChar(std::to_string((int)PultPreferencesKeys::PultLightingH).c_str(), PultLightingColor.h);
    preferences.putUChar(std::to_string((int)PultPreferencesKeys::PultLightingS).c_str(), PultLightingColor.s);
    preferences.putUChar(std::to_string((int)PultPreferencesKeys::PultLightingV).c_str(), PultLightingColor.v);

    preferences.putUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingH).c_str(), GaugesLightingColor.h);
    preferences.putUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingS).c_str(), GaugesLightingColor.s);
    preferences.putUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingV).c_str(), GaugesLightingColor.v);
    
    preferences.putInt(std::to_string((int)PultPreferencesKeys::LogLevel).c_str(), LogLevel);
    // todo
    // NetworkSsid,
    // NetworkPass,
    // NetworkPort,

    preferences.end();
}

// Load preferences from NVS to PultPreferences namespace variables
static void load_pult_preferences()
{
    Preferences preferences;
    preferences.begin("PhysPult", true);

    EnableGaugesLighting = preferences.getBool(std::to_string((int)PultPreferencesKeys::EnableGaugesLighting).c_str(), true);
    EnablePotentiometer = preferences.getBool(std::to_string((int)PultPreferencesKeys::EnablePotentiometer).c_str(), true);

    SyncController = preferences.getBool(std::to_string((int)PultPreferencesKeys::SyncController).c_str(), false);
    SyncReverser = preferences.getBool(std::to_string((int)PultPreferencesKeys::SyncReverser).c_str(), false);
    SyncCrane = preferences.getBool(std::to_string((int)PultPreferencesKeys::SyncCrane).c_str(), false);
    SyncInputRegisters = preferences.getBool(std::to_string((int)PultPreferencesKeys::SyncInputRegisters).c_str(), true);

    PultLightingColor.h = preferences.getUChar(std::to_string((int)PultPreferencesKeys::PultLightingH).c_str(), 25);
    PultLightingColor.s = preferences.getUChar(std::to_string((int)PultPreferencesKeys::PultLightingS).c_str(), 190);
    PultLightingColor.v = preferences.getUChar(std::to_string((int)PultPreferencesKeys::PultLightingV).c_str(), 255);

    GaugesLightingColor.h = preferences.getUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingH).c_str(), 25);
    GaugesLightingColor.s = preferences.getUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingS).c_str(), 190);
    GaugesLightingColor.v = preferences.getUChar(std::to_string((int)PultPreferencesKeys::GaugesLightingV).c_str(), 255);

    SerialBaudRate = preferences.getInt(std::to_string((int)PultPreferencesKeys::SerialBaudRate).c_str(), 115200);
    LogLevel = (esp_log_level_t)preferences.getInt(std::to_string((int)PultPreferencesKeys::LogLevel).c_str(), ESP_LOG_DEBUG);

    log_i("EnableGaugesLighting - %d", EnableGaugesLighting);
    log_i("EnablePotentiometer - %d", EnablePotentiometer);

    log_i("SyncController - %d", SyncController);
    log_i("SyncReverser - %d", SyncReverser);
    log_i("SyncCrane - %d", SyncCrane);
    log_i("SyncInputRegisters - %d", SyncInputRegisters);

    log_i("PultLightingColor - {%d, %d, %d}", PultLightingColor.h, PultLightingColor.s, PultLightingColor.v);
    log_i("GaugesLightingColor - {%d, %d, %d}", GaugesLightingColor.h, GaugesLightingColor.s, GaugesLightingColor.v);

    log_i("SerialBaudRate - %d", SerialBaudRate);
    log_i("LogLevel - %d", LogLevel);
    // todo
    // NetworkSsid,
    // NetworkPass,
    // NetworkPort,

    preferences.end();
}

// Constants
const uint8_t SupplyVoltmeterPwmChannel = 12;
const uint8_t BatteryVoltmeterPwmChannel = 13;
const uint8_t EnginesCurrentNegativePwmChannel = 15;
const uint8_t EnginesCurrentPositivePwmChannel = 14;

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
