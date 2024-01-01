#include "States.h"

State(HardwareInitialization)
{
  InitializeHardware(physPult);

  xTaskCreatePinnedToCore(
    BackgroundHardwareFunction,
    "BackgroundHardwareTask",
    15000,  // Stack size
    &physPult,
    0,  // Priority of the task 
    nullptr, // TaskHandle_t
    0);

  SetState(NetworkInitialization);
}

State(NetworkInitialization)
{
  WiFi.disconnect();
  WiFi.begin(WifiSsid, WifiPassword);

  uint8_t connectionDisplayTimer = 1;
  const uint8_t connectionTimerLimit = 50;

  for(; connectionDisplayTimer <= connectionTimerLimit; connectionDisplayTimer++) 
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      break;
    }

    Serial.print(".");
    delay(100);
  }

  if(connectionDisplayTimer == connectionTimerLimit)
  {
    Serial.print("Couldn't connect to network. Retrying.");
    return;
  }

  auto ip = WiFi.localIP().toString(); 
  Serial.println("\nConnected to network. IPv4:");
  Serial.println(ip);

  // todo: This doesn't work with 3 digits though
  physPult.DeviceNumber = atoi(ip.substring(ip.length() - 2).c_str());

  physPult.Server.listen(NetworkPort);
  Serial.print("Server is ");
  if(physPult.Server.available() == false)
  {
    Serial.print("not ");
  }
  Serial.println("available.");

  SetState(WaitingForClient);
}

State(WaitingForClient)
{
  Serial.println("Waiting for client.");

  // todo: This blocks thread
  // todo: create taskhandle and pass to hardware thread to make it possible to terminate this task. Or repalce with non-blocking version (how?)
  physPult.Client = physPult.Server.accept();

  Serial.println("Client accepted.");

  SetState(Work);
}

State(Work)
{
  static String previousMsg;

  if(physPult.Client.available())
  {
    // todo: This blocks thread
    websockets::WebsocketsMessage websocketMsg = physPult.Client.readBlocking();

    // todo: implement check for msg type

    String msg = websocketMsg.data();

    PhysPultMessage receivedMsg(msg);

    physPult.AcceptMessage(receivedMsg);

    PhysPultMessage msgToSend = physPult.MessageToSend();

    if(msgToSend.ToString() != previousMsg)
    {
      physPult.Client.send(msgToSend.ToString());
      previousMsg = msgToSend.ToString();
    }
  }
  else
  {
    Serial.println("Client disconnected.");
    SetState(WaitingForClient)
  }
}