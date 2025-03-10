#pragma once

#include "PultMessage.h"
#include <memory>

class PultMessage;

typedef std::function<void(std::shared_ptr<PultMessage>)> OnPultMessage;
typedef std::function<void()> OnConnect;
typedef std::function<void()> OnDisconnect;
typedef std::function<void(int)> OnDeviceNumberChanged;

class Communicator
{
    public:
    virtual int get_device_number() = 0;
    virtual void set_on_message(OnPultMessage handler) = 0;
    virtual void set_on_connect(OnConnect handler) = 0;
    virtual void set_on_disconnect(OnDisconnect handler) = 0;
    virtual void set_on_device_number_changed(OnDeviceNumberChanged handler) = 0;
    virtual void send(std::shared_ptr<PultMessage> msg) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};