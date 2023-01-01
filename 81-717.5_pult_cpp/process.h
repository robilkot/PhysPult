#include <string>
#include <iostream>

#pragma once

using namespace std;

string indicators_process(const string& PATH, string& previous);
void switches_process(const string& PATH, const string& current, string& previous);