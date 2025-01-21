#pragma once

#include <Communication/Communicator.h>

class PultMessage;

class SerialCommunicator : public Communicator
{
    private:
    std::function<void(PultMessage&)> on_message;
    std::function<void()> on_disconnect;
    std::function<void()> on_connect;
    std::function<void(int)> on_device_number_changed;

    void move_next();

    public:
    int get_device_number() override;
    void set_on_message(std::function<void(PultMessage&)> handler) override;
    void set_on_connect(std::function<void()> handler) override;
    void set_on_disconnect(std::function<void()> handler) override;
    void set_on_device_number_changed(std::function<void(int)> handler) override;
    void send(PultMessage& msg) override;
    void start() override;
    void stop() override;
};