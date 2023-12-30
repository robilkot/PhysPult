#include <Arduino.h>
#include <ESP32Servo.h>
#include <FastLED.h>
#include <TimerMs.h>
#include <ArduinoWebsockets.h>

#include "Constants.h"
#include "WifiConstants.h"
#include "Hardware.h"
#include "PhysPult.h"

#define State(identifier) void identifier(void*& state, PhysPult& physPult)
#define SetState(identifier) state = (void*)identifier;

State(HardwareInitialization);
State(NetworkInitialization);
State(WaitingForClient);
State(Work);

void setup() { }

void loop()
{
  static PhysPult physPult;
  static void (*state)(void*&, PhysPult&) = HardwareInitialization;
  
  (*state)((void*&)state, physPult);
}

State(HardwareInitialization)
{
  Serial.begin(BaudRate);
  WiFi.begin(WifiSsid, WifiPassword);

  pinMode(InDataPin, INPUT);
  pinMode(InLatchPin, OUTPUT);
  pinMode(InClockPin, OUTPUT);

  pinMode(OutDataPin, OUTPUT);
  pinMode(OutLatchPin, OUTPUT);
  pinMode(OutClockPin, OUTPUT);

  FastLED.addLeds<WS2812B, LightingDataPin, RGB>(physPult.LightingLeds, LightingLedCount);
  FastLED.setBrightness(120);
  for(uint8_t i = 0; i < LightingLedCount; i++)
  {
    physPult.LightingLeds[i] = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
  }
  FastLED.show();

  physPult.TmServo.attach(TmPwmPin); 
  physPult.NmServo.attach(NmPwmPin); 
  physPult.TcServo.attach(TcPwmPin); 

  TaskHandle_t BackgroundHardwareTask;

  xTaskCreatePinnedToCore(
    BackgroundHardwareFunction,
    "BackgroundHardwareTask",
    20000,  /* Stack size */
    &physPult,
    0,  /* Priority of the task */
    &BackgroundHardwareTask,
    0);

  SetState(NetworkInitialization);
}

State(NetworkInitialization)
{
  while(WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      SetIndicatorsOn();
      delay(500);
      SetIndicatorsOff();
      delay(500);
  }

  auto ip = WiFi.localIP().toString(); 
  Serial.println("\nConnected to network. IPv4:");
  Serial.println(ip);

  // todo: Implement proper number extraction. Core panic here for now
  // char* ipString = &(WiFi.localIP().toString()[0]);
  // char* token = strtok(ipString, ".");
  
  // char* deviceNumberString = nullptr;
  // while(token != nullptr)
  // {
  //   deviceNumberString = token;
  //   Serial.println(token);
  //   token = strtok(nullptr, ".");
  // }
  // uint8_t deviceNumber = atoi(deviceNumberString);

  // This doesn't work with 3 digits though
  uint8_t deviceNumber = atoi(ip.substring(ip.length() - 2).c_str());

  uint8_t portIndication[] = { deviceNumber, 255, 255, 255, 255 };
  WriteOutRegisters(portIndication);

  physPult.Server.listen(NetworkPort);
  Serial.print("Server available (0/1)? ");
  Serial.println(physPult.Server.available());

  SetState(WaitingForClient);
}

State(WaitingForClient)
{
  Serial.println("Waiting for client.");

  // todo: This blocks thread
  physPult.Client = physPult.Server.accept();

  Serial.println("Client accepted.");

  SetState(Work);
}

State(Work)
{
  if(physPult.Client.available())
  {
    // todo: This blocks thread
    websockets::WebsocketsMessage msg = physPult.Client.readBlocking();
  
    Serial.print("Got Message: ");
    Serial.println(msg.data());

    // physPult.Client.send("Echo: " + msg.data());
    physPult.Client.send((char*)physPult.Input);
  }
  else
  {
    Serial.println("Client disconnected.");
    SetState(WaitingForClient)
  }
}