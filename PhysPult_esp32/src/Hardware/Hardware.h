#pragma once

#include <ESP32Servo.h>
#include <FastLED.h>
#include <Constants.h>
#include <TimerMs.h>
#include "StableReader.h"

class Hardware
{
    private:
    Servo TmServo; 
    Servo NmServo; 
    Servo TcServo;

    CRGB LightingLeds1[LightingLedCount1];
    CRGB LightingLeds2[LightingLedCount2];
    CRGB GaugesLeds[GaugesLightingLedCount];

    StableReader potentiometer1_reader; // For 1st block
    StableReader potentiometer2_reader; // For 3rd block
    StableReader crane_reader;
    
    uint8_t potentiometer1_position = 0;
    uint8_t potentiometer2_position = 0;

    void adjust_value(uint8_t& currentValue, const uint8_t targetValue)
    {
        if(currentValue > targetValue)
        {
            currentValue--;
        }
        else if(currentValue < targetValue)
        {
            currentValue++;
        }
    }

    void tick_output()
    {
        tick_registers();
        tick_leds();
        tick_servos();
        tick_analog_contols();
    }

    void tick_registers()
    {
        static TimerMs registersUpdateTimer(15, true, false);

        if(registersUpdateTimer.tick())
        {
            digitalWrite(OutLatchPin, 0);
            delayMicroseconds(PulseWidth);

            for(auto reg : registers_out) {
                // log_v("%d", reg);
                shiftOut(OutDataPin, OutClockPin, LSBFIRST, reg);
                vTaskDelay(PulseWidth / portTICK_PERIOD_MS);
            }

            digitalWrite(OutLatchPin, 1);
        }
    }

    void tick_analog_contols()
    {
        static TimerMs analogUpdateTimer(5, true, false);

        if(analogUpdateTimer.tick())
        {
            // Battery voltmeter
            ledcWrite(13, battery_voltage);

            // Supply voltmeter
            ledcWrite(12, supply_voltage);

            // Engines ampmeter
            uint8_t enginesCurrentAbsolute = abs(engines_current);
            
            if(engines_current > 0)
            {
                ledcWrite(14, 0);
                ledcWrite(15, enginesCurrentAbsolute);
            }
            else
            {
                ledcWrite(14, enginesCurrentAbsolute);
                ledcWrite(15, 0);
            }
        }
    }

    void tick_servos()
    {
        static uint8_t tmCurrentValue = 0;
        static uint8_t nmCurrentValue = 0;
        static uint8_t tcCurrentValue = 0;
        
        // To prevent too fast servos movements
        static TimerMs servoUpdateTimer(5, true, false);

        if(servoUpdateTimer.tick())
        {
            adjust_value(tmCurrentValue, tm_position);
            adjust_value(nmCurrentValue, nm_position);
            adjust_value(tcCurrentValue, tc_position);
            
            // Adjustments caused by physical behaviour of servos
            TmServo.write(map(tmCurrentValue, 0, 255, 0, 180));
            NmServo.write(255 - map(nmCurrentValue, 0, 255, 80, 255));
            TcServo.write(tcCurrentValue);
        }
    }

    void tick_leds()
    {
        for(auto& led : LightingLeds1)
            led = CHSV(LightingColorHue, LightingColorSat, potentiometer1_position * lighting_enabled);
        
        for(auto& led : LightingLeds2)
            led = CHSV(LightingColorHue, LightingColorSat, potentiometer2_position * lighting_enabled);

        for(auto& led : GaugesLeds)
            led = CHSV(LightingColorHue, LightingColorSat, potentiometer1_position * lighting_enabled);

        FastLED.show();
    }

    void tick_input()
    {
        potentiometer1_position = potentiometer1_reader.tick();
        potentiometer2_position = potentiometer2_reader.tick();
        crane_position = crane_reader.tick();

        digitalWrite(InLatchPin, LOW);    
        delayMicroseconds(PulseWidth);
        digitalWrite(InLatchPin, HIGH);

        for(uint8_t i = 0; i < InRegistersCount; i++)
        {
            registers_in[i] = shiftIn(InDataPin, InClockPin, LSBFIRST);
        }
    }

    void init_pins()
    {   
        pinMode(OutDataPin, OUTPUT);
        pinMode(OutLatchPin, OUTPUT);
        pinMode(OutClockPin, OUTPUT);

        pinMode(InDataPin, INPUT);
        pinMode(InLatchPin, OUTPUT);
        pinMode(InClockPin, OUTPUT);

        pinMode(BatteryVoltmeterPwmPin, OUTPUT);
        pinMode(SupplyVoltmeterPwmPin, OUTPUT);
        pinMode(EnginesCurrentPositivePwmPin, OUTPUT);
        pinMode(EnginesCurrentNegativePwmPin, OUTPUT);

        pinMode(LightingDataPin1, OUTPUT);
        pinMode(LightingDataPin2, OUTPUT);
        pinMode(GaugesLightingDataPin, OUTPUT);

        pinMode(TmPwmPin, OUTPUT);
        pinMode(TmPwmPin, OUTPUT);
        pinMode(TcPwmPin, OUTPUT);

        pinMode(PotentiometerPin1, INPUT);
        pinMode(PotentiometerPin2, INPUT);
        pinMode(CranePin, INPUT);
    }

    void init()
    {
        init_pins();

        FastLED.addLeds<WS2812B, LightingDataPin1, GRB>(LightingLeds1, sizeof(LightingLeds1) / sizeof(CRGB));
        // todo: crash
        // FastLED.addLeds<WS2812B, LightingDataPin2, GRB>(LightingLeds2, sizeof(LightingLeds2) / sizeof(CRGB));
        // FastLED.addLeds<WS2812B, GaugesLightingDataPin, GRB>(GaugesLeds, sizeof(GaugesLeds) / sizeof(CRGB));
        FastLED.setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(255);

        // todo: document + move pwm channels to constants.h
        ledcSetup(12, 5000, 8);
        ledcSetup(13, 5000, 8);
        ledcSetup(14, 5000, 8);
        ledcSetup(15, 5000, 8);
        ledcAttachPin(EnginesCurrentNegativePwmPin, 14);
        ledcAttachPin(EnginesCurrentPositivePwmPin, 15);
        ledcAttachPin(SupplyVoltmeterPwmPin, 12);
        ledcAttachPin(BatteryVoltmeterPwmPin, 13);

        TmServo.attach(TmPwmPin);
        NmServo.attach(NmPwmPin);
        TcServo.attach(TcPwmPin);
        
        log_i("Hardware initialization complete");
    }

    public:
    uint8_t registers_in[InRegistersCount];
    uint8_t registers_out[OutRegistersCount];

    uint8_t tm_position = 0;
    uint8_t nm_position = 0;
    uint8_t tc_position = 0;

    uint8_t battery_voltage = 0;

    uint8_t supply_voltage = 0;
    int16_t engines_current = 0; // -255..255

    uint8_t crane_position = 0;

    bool lighting_enabled = false;

    Hardware()
    : crane_reader(StableReader(CranePin)), potentiometer1_reader(StableReader(PotentiometerPin1)), potentiometer2_reader(StableReader(PotentiometerPin2))
    {
        for(auto& led : LightingLeds1)
            led = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);

        for(auto& led : LightingLeds2)
            led = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);

        for(auto& led : GaugesLeds)
            led = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
    }

    void start()
    {
        init();
        
        while(true)
        {
            tick_input();
            tick_output();
        }
    }
};