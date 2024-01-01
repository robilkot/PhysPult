#include <Arduino.h>

#include "PhysPult.h"
#include "States.h"

void setup() { }

void loop()
{
  static PhysPult physPult(HardwareInitialization);
  
  (*physPult.State)(physPult);
}