#include <string>
#include <iostream>
#include <fstream>
#include "include/SimpleSerial.h"

#pragma once

std::string indicatorsProcess(const std::string& PATH, std::string& previous);
void switchesProcess(const std::string& PATH, const std::string& current, std::string& previous);
void updateControls(SimpleSerial& Serial, const std::string& indicatorsPATH, std::string& indicatorsprevious, const std::string& switchesPATH, std::string& switchessprevious);