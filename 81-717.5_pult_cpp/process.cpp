#include <string>
#include <iostream>
#include <fstream>

//#define USECHANGEDSTATEMODEL 
// Использовать если нужно преобразовать строку состояния в строку изменения состояния (legacy)

using namespace std;

string indicators_process(const string& PATH, string& previous)
{
    ifstream in(PATH);
    if (!in.is_open()) {
        cerr << "Couldnt open file!\n";
        system("pause");
        return {};
    }
    string state;
    getline(in, state);
    if (previous.length() != state.length()) {
        cerr << "Error processing indicators state (unequal strings length)!\n";
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

void switches_process(const string& PATH, const string& current, string& previous)
{
    if (previous.length() != current.length()) {
        cerr << "Error processing indicators state (unequal strings length), skipping\n";
        return ; // Выход если предыдущая строка не совпадает по длине с данной.
    }

    if (previous == current) return;
    previous = current;

    ofstream out(PATH);
    if (!out.is_open()) {
        cerr << "Couldnt save file!\n";
        system("pause");
        return;
    }
    out << current;
}