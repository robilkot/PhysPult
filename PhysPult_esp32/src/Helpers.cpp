#include "Helpers.h"

void yieldIfNecessary(void){
    static uint64_t lastYield = 0;
    uint64_t now = millis();
    if((now - lastYield) > 1500) {
        lastYield = now;
        vTaskDelay(5); //delay 1 RTOS tick
    }
}

void adjust_value(int& currentValue, const int targetValue, const int step)
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