#pragma once

#include <ArduinoWebsockets.h>
#include <TimerMs.h>

#include "PhysPult.h"
#include "WifiConstants.h"
#include "Constants.h"
#include "Hardware.h"
#include "PhysPultMessage.h"

#define State(identifier) void identifier(PhysPult& physPult)
#define SetState(identifier) physPult.State = identifier; physPult.MessageNumber = 0;

State(HardwareInitialization);
State(NetworkInitialization);
State(WaitingForClient);
State(Work);