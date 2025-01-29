#include <Arduino.h>
#include "Config.h"
#include "Pult.h"
#include "Communication/WebsocketsCommunicator/WebsocketsCommunicator.h"
#include "Communication/SerialCommunicator/SerialCommunicator.h"

void setup()
{
  Serial.begin(BaudRate);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", LogLevel);

  auto communicator = std::shared_ptr<Communicator>(new WebsocketsCommunicator);
  // auto communicator = std::shared_ptr<Communicator>(new SerialCommunicator);
  Pult::set_communicator(communicator);
  
  Pult::start();
}

void loop()
{
}