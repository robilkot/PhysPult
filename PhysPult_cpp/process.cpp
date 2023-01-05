#include <string>
#include <iostream>
#include <fstream>
#include "include/SimpleSerial.h"

//#define USECHANGEDSTATEMODEL 
// Использовать если нужно преобразовать строку состояния в строку изменения состояния (legacy)

std::string indicatorsProcess(const std::string& PATH, std::string& previous)
{
    using namespace std;

    ifstream in(PATH);
    if (!in.is_open()) {
        cerr << "Error opening indicators state file, skipping\n";
        return {};
    }
    string state;
    getline(in, state);
    if (previous.length() != state.length()) {
        cerr << "Error processing indicators state (unequal strings length), skipping\n";
        return {}; // Возврат пустой строки если предыдущая строка не совпадает по длине с данной. todo: выяснить причину возникновения такой ситуации
    }

#ifdef USECHANGEDSTATEMODEL
    string output(state.length(), '0'); // Формируем выходную строку, изначально все лампы неизменны - поэтому 0

    bool empty = 1;
    for (short i = 0; i < state.length(); i++) {
        if (state[i] != previous[i]) {
            empty = 0;
            if (state[i] == '1') output[i] = '1'; // Если надо включить индикатор - 1
            else output[i] = '2'; // Если выключить - 2
        }
    }

    previous = state; // Пишем актуальное состояние в предыдущее
    if (empty) return {};
    return output;

#else
    if (previous == state) return {};
    previous = state;
    return state;
#endif
}

void switchesProcess(const std::string& PATH, const std::string& current, std::string& previous)
{
    using namespace std;

    if (previous.length() != current.length()) {
        cerr << "Error processing switches state (unequal strings length), skipping\n";
        return ; // Выход если предыдущая строка не совпадает по длине с данной.
    }

    if (previous == current) return;
    previous = current;

    ofstream out(PATH);
    if (out.is_open()) out << current;
    else cerr << "Error saving switches state, skipping\n";
}

void updateControls(SimpleSerial& Serial, const std::string& indicatorsPATH, std::string& indicatorsprevious, const std::string& switchesPATH, std::string& switchessprevious)
{
    using namespace std;

    string changedstate = "{" + indicatorsProcess(indicatorsPATH, indicatorsprevious) + "}";
    if (changedstate.size() > 2) {
        cout << "wrt " << changedstate << Serial.WriteSerialPort(&changedstate[0]) << "\n";
    }

    string rec = Serial.ReadSerialPort(1, "json");
    if (!rec.empty()) {
        cout << "rec {" << rec << "}\n";
        switchesProcess(switchesPATH, rec, switchessprevious);
    }
}