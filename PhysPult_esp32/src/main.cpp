#include <Arduino.h>
#include "Constants.h"
#include "Pult.h"


void setup()
{
  Serial.begin(BaudRate);

  Pult pult;
  pult.start();
}

void loop()
{

}