#pragma once

#include <functional>
#include <ArduinoWebsockets.h>
#include <WifiConstants.h>
#include <Config.h>
#include "PultMessageFactory.h"

class PultMessage;

class Communicator
{
    private:
    std::function<void(PultMessage&)> on_message;
    std::function<void()> on_disconnect;
    std::function<void()> on_connect;
    std::function<void(int)> on_ip_changed;
    
    websockets::WebsocketsServer server;
    websockets::WebsocketsClient client;

    void accept_client();
    void connect_to_network();

    public:
    uint8_t device_number;

    Communicator();
    void set_on_message(std::function<void(PultMessage&)> handler);
    void set_on_connect(std::function<void()> handler);
    void set_on_disconnect(std::function<void()> handler);
    void set_on_ip_changed(std::function<void(int)> handler);
    void send(PultMessage& msg);
    void start();
};