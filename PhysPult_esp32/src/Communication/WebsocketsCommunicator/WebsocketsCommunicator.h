#pragma once

#include <ArduinoWebsockets.h>
#include <Config.h>
#include <WifiConstants.h>
#include <Communication/Communicator.h>
#include <Communication/PultMessageFactory.h>
#include "Helpers.h"

class WebsocketsCommunicator : public Communicator
{
    private:
    OnPultMessage on_message;
    OnDisconnect on_disconnect;
    OnConnect on_connect;
    OnDeviceNumberChanged on_ip_changed;
    TaskHandle_t communcation_task;
    
    uint8_t device_number;

    websockets::WebsocketsServer server;
    websockets::WebsocketsClient client;

    void accept_client();
    void connect_to_network();

    public:
    int get_device_number() override;
    void set_on_message(OnPultMessage handler) override;
    void set_on_connect(OnConnect handler) override;
    void set_on_disconnect(OnDisconnect handler) override;
    void set_on_device_number_changed(OnDeviceNumberChanged handler) override;
    void send(std::shared_ptr<PultMessage> msg) override;
    void start() override;
    void stop() override;
};