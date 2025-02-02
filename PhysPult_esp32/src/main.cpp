#include <Arduino.h>
#include "Config.h"
#include "Pult.h"
#include "Communication/WebsocketsCommunicator/WebsocketsCommunicator.h"
#include "Communication/SerialCommunicator/SerialCommunicator.h"
#include "PultPreferences.h"


void setup()
{
  esp_log_level_set("*", LogLevel);
  Serial.begin(SerialBaudRate);
  Serial.setDebugOutput(true);

  // auto communicator = std::shared_ptr<Communicator>(new WebsocketsCommunicator);
  auto communicator = std::shared_ptr<Communicator>(new SerialCommunicator);
  Pult::set_communicator(communicator);
  
  Pult::start();
}

void loop()
{
}