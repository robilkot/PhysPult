#pragma once

#include <Arduino.h>
#include <vector>

enum PhysPultMessageTypes {
    INFO = 'I', // Information message sent
    WORKING = 'W', // Operating mode. Exchanging indicators and switches states
    REQUEST = 'R' // Update of switches state requsted. No data parsing required
    // ERROR = 'E', // Error message sent
    // SETTINGS = 'S', // Settings sent
};

struct PhysPultMessage {
    const char Delimiter = ';'; 

    char Type = PhysPultMessageTypes::INFO;
    std::vector<int16_t> NumericData;
    std::vector<uint8_t> BinaryData;

    String ToString();

    PhysPultMessage();
    
    PhysPultMessage(String);
};