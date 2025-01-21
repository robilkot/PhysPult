#pragma once

#include <Communication/Communicator.h>

class PultMessage;

class SerialCommunicator : public Communicator
{
    private:
    OnPultMessage on_message;
    OnDisconnect on_disconnect;
    OnConnect on_connect;
    OnDeviceNumberChanged on_device_number_changed;

    void move_next();

    public:
    int get_device_number() override;
    void set_on_message(OnPultMessage handler) override;
    void set_on_connect(OnConnect handler) override;
    void set_on_disconnect(OnDisconnect handler) override;
    void set_on_device_number_changed(OnDeviceNumberChanged handler) override;
    void send(const PultMessage& msg) override;
    void start() override;
    void stop() override;
};