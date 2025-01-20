#pragma once

#include <Arduino.h>
#include <vector>
#include "Pult.h"

class Pult;

enum class OutputStateKeys {
    Controller,
    Reverser,
    Crane,
};

enum class InputStateKeys {
    Speed,
    TM,
    NM,
    TC,
    BatteryVoltage,
    SupplyVoltage, // KVMeter
    EnginesCurrent, // Ampermeter
};

union StateKeys {
    InputStateKeys input;
    OutputStateKeys output;
};

enum class DebugActions {
    OK,
    ERROR,
    RESET,
    REGISTER_OUT_ENABLE,
    REGISTER_OUT_DISABLE,
    TOGGLE_LIGHTING
};


class PultMessage {
    public:
    const virtual char get_type() = 0;
    virtual void apply(Pult& pult) = 0;
    virtual String to_string() {
        String output(get_type());
        output += ';';
        return output;
    };
};

class StateRequestMessage : public PultMessage {
    public:
    const char get_type() override {
        return 'R';
    }
    void apply(Pult& pult) override;
};

class StateChangePultMessage : public PultMessage {
    public:
    std::vector<uint8_t> pins_enabled;
    std::vector<uint8_t> pins_disabled;
    std::vector<std::pair<StateKeys, int16_t>> new_values;

    String to_string() override;
    const char get_type() override {
        return 'S';
    }
    void apply(Pult& pult) override;
};

class WorkPultMessage : public PultMessage {
    public:
    std::vector<int16_t> numeric_data;
    std::vector<uint8_t> binary_data;

    String to_string() override;
    const char get_type() override {
        return 'W';
    }
    void apply(Pult& pult) override;
};

class ConfigPultMessage : public PultMessage {
    public:
    String key;
    String value;

    const char get_type() override {
        return 'C';
    }
    void apply(Pult& pult) override;
};

class DebugPultMessage : public PultMessage {
    public:
    DebugActions action;
    std::vector<int16_t> params;

    String to_string() override;
    const char get_type() override {
        return 'D';
    }
    void apply(Pult& pult) override;
};