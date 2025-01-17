#pragma once

#include <Arduino.h>
#include <vector>

enum PultMessageTypes {
    WORKING = 'W', // Operating mode. Exchanging indicators and switches states
    DEBUG = 'D'
};

struct PultMessage {
    const char Delimiter = ';'; 

    char Type = PultMessageTypes::WORKING;
    std::vector<int16_t> NumericData;
    std::vector<uint8_t> BinaryData;

    String ToString();

    PultMessage();
    
    PultMessage(String);
};