#pragma once

#include <ESP32Servo.h>
#include <FastLED.h>
#include <Config.h>
#include <TimerMs.h>
#include <mutex>
#include "StableReader.h"
#include "esp_task_wdt.h"
#include <Helpers.h>

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

    bool pult_lighting_on;

    static void adjust_value(int& currentValue, const int targetValue, const int step = 1)
    {
        if(currentValue == targetValue) {
            return;
        }

        auto diff = min(abs(currentValue - targetValue), step);

        if(currentValue > targetValue) {
            currentValue -= diff;
        } else {
            currentValue += diff;
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
            std::lock_guard<std::mutex> lock(mutex);
            
            digitalWrite(OutLatchPin, 0);

            for(auto reg : registers_out) {
                shiftOut(OutDataPin, OutClockPin, LSBFIRST, reg);
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
        static int tmCurrentValue = 0;
        static int nmCurrentValue = 0;
        static int tcCurrentValue = 0;
        
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
        static int current_lighting_brightness = 0;
        static TimerMs display_timer(25, true, false);
        static TimerMs math_timer(15, true, false);

        if(math_timer.tick()) {
            int target_lighting_brightness = (int)(potentiometer1_position * (pult_lighting_on ^ invert_lighting));
            adjust_value(current_lighting_brightness, target_lighting_brightness, 20);

            auto pult_v = (uint8_t)(PultLightingColor.v * (current_lighting_brightness / 255.));
            auto gauges_v = (uint8_t)(GaugesLightingColor.v * (current_lighting_brightness / 255.) * gauges_lighting_on);

            for(auto& led : LightingLeds1)
                led = CHSV(PultLightingColor.h, PultLightingColor.s, pult_v);
            
            for(auto& led : LightingLeds2)
                led = CHSV(PultLightingColor.h, PultLightingColor.s, pult_v);

            for(auto& led : GaugesLeds)
                led = CHSV(GaugesLightingColor.h, GaugesLightingColor.s, gauges_v);
        }

        if(display_timer.tick())
        {
            FastLED.show();
        }
    }

    void tick_input()
    {
        if(disable_potentiometers) {
            potentiometer1_position = 255;
            potentiometer2_position = 255;
        }
        else {
            potentiometer1_position = potentiometer1_reader.tick();
            potentiometer2_position = potentiometer2_reader.tick();
        }
        
        crane_position = crane_reader.tick();

        digitalWrite(InLatchPin, LOW);
        digitalWrite(InLatchPin, HIGH);

        for(uint8_t i = 0; i < InRegistersCount; i++)
        {
            registers_in[i] = shiftIn(InDataPin, InClockPin, LSBFIRST);
        }
        
        pult_lighting_on = !(registers_in[2] >> 5 & 1); // Defined by hardware connections
    }

    void init()
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

        FastLED.addLeds<WS2812B, LightingDataPin1, GRB>(LightingLeds1, sizeof(LightingLeds1) / sizeof(CRGB));
        FastLED.addLeds<WS2812B, GaugesLightingDataPin, GRB>(GaugesLeds, sizeof(GaugesLeds) / sizeof(CRGB));
        // todo: crash
        // FastLED.addLeds<WS2812B, LightingDataPin2, GRB>(LightingLeds2, sizeof(LightingLeds2) / sizeof(CRGB));
        FastLED.setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(255);

        ledcSetup(EnginesCurrentNegativePwmChannel, 5000, 8);
        ledcSetup(EnginesCurrentPositivePwmChannel, 5000, 8);
        ledcSetup(SupplyVoltmeterPwmChannel, 5000, 8);
        ledcSetup(BatteryVoltmeterPwmChannel, 5000, 8);
        ledcAttachPin(EnginesCurrentNegativePwmPin, EnginesCurrentNegativePwmChannel);
        ledcAttachPin(EnginesCurrentPositivePwmPin, EnginesCurrentPositivePwmChannel);
        ledcAttachPin(SupplyVoltmeterPwmPin, SupplyVoltmeterPwmChannel);
        ledcAttachPin(BatteryVoltmeterPwmPin, BatteryVoltmeterPwmChannel);

        TmServo.attach(TmPwmPin);
        NmServo.attach(NmPwmPin);
        TcServo.attach(TcPwmPin);
    }

    public:
    static std::mutex mutex;
    
    uint8_t registers_in[InRegistersCount];
    uint8_t registers_out[OutRegistersCount];

    uint8_t tm_position = 0;
    uint8_t nm_position = 0;
    uint8_t tc_position = 0;

    uint8_t battery_voltage = 0;

    uint8_t supply_voltage = 0;
    int16_t engines_current = 0; // -255..255

    uint8_t crane_position = 0;

    bool gauges_lighting_on;
    bool invert_lighting;
    bool disable_potentiometers;

    Hardware()
    : crane_reader(StableReader(CranePin)), potentiometer1_reader(StableReader(PotentiometerPin1)), potentiometer2_reader(StableReader(PotentiometerPin2))
    {
        for(auto& led : LightingLeds1)
            led = CHSV();

        for(auto& led : LightingLeds2)
            led = CHSV();

        for(auto& led : GaugesLeds)
            led = CHSV();
    }

    void set_output(uint8_t out_index, bool value) {
        auto register_index = out_index / 8;
        auto pin_index = out_index % 8;

        if(value) {
            registers_out[register_index] |= (1 << pin_index);
        } else {
            registers_out[register_index] &= ~(1 << pin_index);
        }
    }

    void clear_output()
    {
        for(auto& reg : registers_out) {
            reg = 0;
        }
    }

    void start()
    {
        init();

        while(true)
        {
            yieldIfNecessary();
            tick_input();
            tick_output();
        }
    }
};