#pragma once

#include <ESP32Servo.h>
#include <FastLED.h>
#include <Config.h>
#include <TimerMs.h>
#include <mutex>
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

    static void adjust_value(uint8_t& currentValue, const uint8_t targetValue, int step = 1)
    {
        if(currentValue > targetValue)
        {
            currentValue -= step;
        }
        else if(currentValue < targetValue)
        {
            currentValue += step;
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
        static uint8_t current_brightness = 0;
        static TimerMs update_timer(10, true, false);

        if(update_timer.tick()) 
        {
            bool lighting_switch_on = !(registers_in[2] >> 5 & 1); // Defined by hardware connections

            auto target_brightness = potentiometer1_position / 255. * (toggle_lighting ^ lighting_switch_on) * LightingColor.v;
            adjust_value(current_brightness, target_brightness, 15);

            for(auto& led : LightingLeds1)
                led = CHSV(LightingColor.h, LightingColor.s, current_brightness);
            
            for(auto& led : LightingLeds2)
                led = CHSV(LightingColor.h, LightingColor.s, current_brightness);

            for(auto& led : GaugesLeds)
                led = CHSV(LightingColor.h, LightingColor.s, current_brightness);

            FastLED.show();
        }
    }

    void tick_input()
    {
        potentiometer1_position = potentiometer1_reader.tick();
        potentiometer2_position = potentiometer2_reader.tick();
        crane_position = crane_reader.tick();

        digitalWrite(InLatchPin, LOW);
        digitalWrite(InLatchPin, HIGH);

        for(uint8_t i = 0; i < InRegistersCount; i++)
        {
            registers_in[i] = shiftIn(InDataPin, InClockPin, LSBFIRST);
        }
    }

    void init_pins()
    {   
        pinMode(OutputEnable, OUTPUT);
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
        digitalWrite(OutputEnable, 1);

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
        
        digitalWrite(OutputEnable, 0);
        log_i("Hardware initialization complete");
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

    bool toggle_lighting = false;

    Hardware()
    : crane_reader(StableReader(CranePin)), potentiometer1_reader(StableReader(PotentiometerPin1)), potentiometer2_reader(StableReader(PotentiometerPin2))
    {
        for(auto& led : LightingLeds1)
            led = LightingColor;

        for(auto& led : LightingLeds2)
            led = LightingColor;

        for(auto& led : GaugesLeds)
            led = LightingColor;
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
            tick_input();
            tick_output();
        }
    }
};