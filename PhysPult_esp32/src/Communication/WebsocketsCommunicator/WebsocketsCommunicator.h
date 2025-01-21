#pragma once

#include <ArduinoWebsockets.h>
#include <Config.h>
#include <WifiConstants.h>
#include <Communication/Communicator.h>
#include <Communication/PultMessageFactory.h>

class WebsocketsCommunicator : public Communicator
{
    private:
    std::function<void(PultMessage&)> on_message;
    std::function<void()> on_disconnect;
    std::function<void()> on_connect;
    std::function<void(int)> on_ip_changed;
    
    uint8_t device_number;

    websockets::WebsocketsServer server;
    websockets::WebsocketsClient client;

    void accept_client();
    void connect_to_network();

    public:
    WebsocketsCommunicator();
    int get_device_number() override;
    void set_on_message(std::function<void(PultMessage&)> handler) override;
    void set_on_connect(std::function<void()> handler) override;
    void set_on_disconnect(std::function<void()> handler) override;
    void set_on_device_number_changed(std::function<void(int)> handler) override;
    void send(PultMessage& msg) override;
    void start() override;
};