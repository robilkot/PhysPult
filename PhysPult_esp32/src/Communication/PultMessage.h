#pragma once

#include <Arduino.h>
#include <vector>
#include <memory>
#include "Pult.h"
#include <FeatureFlags.h>

class Pult;

enum class InputStateKeys {
    Controller,
    Reverser,
    Crane,
};

enum class OutputStateKeys {
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

class PultMessage {
    public:
    const virtual char get_type() const = 0;
    virtual void apply() const = 0;
    virtual std::string to_string() const {
        std::string output{get_type()};
        output += ';';
        return output;
    };
};

class StateRequestMessage : public PultMessage {
    public:
    const char get_type() const override {
        return 'R';
    }
    void apply() const  override;
};

class StateChangePultMessage : public PultMessage {
    public:
    std::vector<uint8_t> pins_enabled;
    std::vector<uint8_t> pins_disabled;
    std::vector<std::pair<StateKeys, int16_t>> new_values;

    std::string to_string() const override;
    const char get_type() const override {
        return 'S';
    }
    void apply() const  override;
};

enum class ConfigActions {
    ENABLE_FEATURE,
    DISABLE_FEATURE,
    SET_PULT_LIGHTING_H,
    SET_PULT_LIGHTING_S,
    SET_PULT_LIGHTING_V,
    SET_GAUGES_LIGHTING_H,
    SET_GAUGES_LIGHTING_S,
    SET_GAUGES_LIGHTING_V,
};

union ConfigValue {
    int number;
    FeatureFlags feature_flag;
};

class ConfigPultMessage : public PultMessage {
    public:
    std::vector<std::pair<ConfigActions, ConfigValue>> values;

    const char get_type() const override {
        return 'C';
    }
    void apply() const override;
};

enum class DebugActions {
    OK,
    ERROR,
    RESET,
    REGISTER_OUT_ENABLE,
    REGISTER_OUT_DISABLE,
    TOGGLE_LIGHTING
};

class DebugPultMessage : public PultMessage {
    public:
    DebugActions action;
    std::vector<int16_t> params;

    static std::shared_ptr<DebugPultMessage> Ok()
    {
        auto msg = std::make_shared<DebugPultMessage>();
        msg->action = DebugActions::OK;
        return msg;
    }
    static std::shared_ptr<DebugPultMessage> Error()
    {
        auto msg = std::make_shared<DebugPultMessage>();
        msg->action = DebugActions::ERROR;
        return msg;
    }

    std::string to_string() const override;
    const char get_type() const override {
        return 'D';
    }
    void apply() const override;
};