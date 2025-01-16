#pragma once

#include <Arduino.h>
#include <vector>

enum PhysPultMessageTypes {
    WORKING = 'W', // Operating mode. Exchanging indicators and switches states
    DEBUG = 'D'
    // ERROR = 'E', // Error message sent
    // SETTINGS = 'S', // Settings sent
};

struct PhysPultMessage {
    const char Delimiter = ';'; 

    char Type = PhysPultMessageTypes::WORKING;
    std::vector<int16_t> NumericData;
    std::vector<uint8_t> BinaryData;

    String ToString();

    PhysPultMessage();
    
    PhysPultMessage(String);
};