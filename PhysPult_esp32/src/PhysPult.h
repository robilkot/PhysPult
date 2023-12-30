#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <FastLED.h>
#include <ESP32Servo.h>

#include "Constants.h"

#pragma once

class PhysPult {
public:
    websockets::WebsocketsServer Server;
    websockets::WebsocketsClient Client;

    Servo TmServo; 
    Servo NmServo; 
    Servo TcServo;

    CRGB LightingLeds[LightingLedCount];

    uint8_t Output[OutRegistersCount];
    uint8_t Input[InRegistersCount];
};