#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <FastLED.h>
#include <ESP32Servo.h>

#include "Constants.h"
#include "PhysPultMessage.h"

#pragma once

struct PhysPult
{
    void (*State)(PhysPult&);

    websockets::WebsocketsServer Server;
    websockets::WebsocketsClient Client;

    Servo TmServo; 
    Servo NmServo; 
    Servo TcServo;

    uint8_t DeviceNumber = 0; // Used to display ipv4 of device
    uint8_t MessageNumber = 0; // Used to display error codes and progress
    
    CRGB LightingLeds[LightingLedCountTotal];
    
    uint8_t LightingBrightness1 = 0; // For 3rd block
    // uint8_t LightingBrightness2 = 0; // For 1st block

    // Output variables (ordering in message as declared here)

    uint8_t Speed = 0; // 0..255

    uint8_t TmValue = 0; // 0..255
    uint8_t NmValue = 0; // 0..255
    uint8_t TcValue = 0; // 0..255

    uint8_t BatteryVoltage = 0; // 0..255

    uint8_t SupplyVoltage = 0; // 0..255
    int16_t EnginesCurrent = 0; // todo: implement

    // Input variables

    // Commented out since controller position is determined by 3 contacts connected to 74hc165 registers
    // uint8_t ControllerPosition = 4; // 1..7
    uint8_t CranePosition = 0; // 0..255 to allow setting intermediate position like irl

    uint8_t InRegisters[InRegistersCount];
    uint8_t OutRegisters[OutRegistersCount];

    PhysPult(void (*)(PhysPult&));

    void AcceptMessage(PhysPultMessage);

    PhysPultMessage MessageToSend();
};