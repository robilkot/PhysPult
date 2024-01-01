#pragma once

#include <Arduino.h>

enum PhysPultMessageTypes {
    WORKING = 'W',
    ERROR = 'E',
    INFO = 'I',
    SETTINGS = 'S'
};

struct PhysPultMessage {
    const char Delimiter = ';'; 

    char Type = PhysPultMessageTypes::WORKING;
    String NumericData;
    String BinaryData;

    String ToString();

    PhysPultMessage();
    
    PhysPultMessage(String);
};