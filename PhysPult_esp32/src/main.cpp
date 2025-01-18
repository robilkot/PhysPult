#include <Arduino.h>
#include "Constants.h"
#include "Pult.h"


void setup()
{
  Serial.begin(BaudRate);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_DEBUG);

  Pult pult;
  pult.start();
}

void loop()
{

}