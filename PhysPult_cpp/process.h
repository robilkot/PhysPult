#include <string>
#include <iostream>

#pragma once

std::string indicators_process(const std::string& PATH, std::string& previous);
void switches_process(const std::string& PATH, const std::string& current, std::string& previous);