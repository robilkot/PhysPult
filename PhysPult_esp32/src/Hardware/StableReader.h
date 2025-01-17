#pragma once

#include <Arduino.h>

class StableReader
{
    private:
    const uint8_t readings_count;
    const uint8_t pin;

    public:
    StableReader(uint8_t analog_pin, uint8_t readings_count = 15)
        : pin(analog_pin), readings_count(readings_count) 
        { }

    int tick()
    {
        static int reading = 0;
        static uint8_t reading_index = 0;
        static uint8_t new_reading = 0;
        static uint32_t accumulator = 0;

        // Prevents from flickering since measurements are not single. Doesn't block execution performing multiple analogReads per call.
        if(reading_index < readings_count)
        {
            accumulator += analogRead(pin);
            reading_index++;
        }
        else
        {
            accumulator /= (reading_index + 1);

            new_reading = map(accumulator, 0, 4096, 255, 0);
            accumulator = 0;
            reading_index = 0;

            // Prevents flickering because of inaccurate measurements (even with multiple analogRead)
            auto delta = new_reading - reading;
            
            if(abs(delta) > 2)
            {
                reading = new_reading;
            }
        }

        return reading;
    }
};