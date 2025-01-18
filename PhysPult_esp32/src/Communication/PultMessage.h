#pragma once

#include <Arduino.h>
#include <vector>
#include "Pult.h"

class PultMessage {
    private:
    public:
    virtual String to_string() = 0;
    virtual char get_type() = 0;
};

class WorkPultMessage : public PultMessage {
    public:
    std::array<int16_t, 16> numeric_data;
    std::array<uint8_t, InRegistersCount> binary_data;

    String to_string() override {
        String output(get_type());
        output += ';';

        for(auto i : numeric_data)
        {
            output += String(i);
            output += ',';
        }

        output[output.length() - 1] = ';';

        for(auto i : binary_data)
        {
            output += String(i);
            output += ',';
        }

        output[output.length() - 1] = ';';

        return output;
    }

    char get_type() override {
        return 'W';
    }
};

class ConfigPultMessage : public PultMessage {
    public:
    String key;
    String value;

    String to_string() override {
        String output(get_type());
        output += ';';
        return output;
    }

    char get_type() override {
        return 'C';
    }
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

    String to_string() override {
        String output(get_type());
        output += ';';
        output += (int)action;
        output += ';';
        for(auto param : params) {
            output += (int)action;
            output += ',';
        }
        output[output.length() - 1] = ';';
        return output;
    }

    char get_type() override {
        return 'D';
    }
};