#include <Arduino.h>
#include "Config.h"
#include "Pult.h"


void setup()
{
  Serial.begin(BaudRate);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", LogLevel);

  Pult pult;
  pult.start();
}

void loop()
{

}