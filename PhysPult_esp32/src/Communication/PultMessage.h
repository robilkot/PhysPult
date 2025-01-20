#pragma once

#include <Arduino.h>
#include <vector>
#include "Pult.h"

class Pult;

class PultMessage {
    public:
    virtual String to_string() = 0;
    virtual void apply(Pult& pult) = 0;
    virtual char get_type() = 0;
};

class WorkPultMessage : public PultMessage {
    public:
    std::vector<int16_t> numeric_data;
    std::vector<uint8_t> binary_data;

    String to_string() override;
    char get_type() override;
    void apply(Pult& pult) override;
};

class ConfigPultMessage : public PultMessage {
    public:
    String key;
    String value;

    String to_string() override;
    char get_type() override;
    void apply(Pult& pult) override;
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

    String to_string() override;
    char get_type() override;
    void apply(Pult& pult) override;
};