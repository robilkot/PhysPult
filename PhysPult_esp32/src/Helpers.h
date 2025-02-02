#pragma once

#include <Arduino.h>

void yieldIfNecessary();

void adjust_value(int& currentValue, const int targetValue, const int step = 1);