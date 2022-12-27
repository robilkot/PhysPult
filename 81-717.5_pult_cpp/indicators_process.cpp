#include <string>
#include <iostream>
#include <fstream>

//#define DEBUG

using namespace std;

string indicators_change(const string& PATH, string& previous) {
    ifstream in(PATH);
    if (!in.is_open()) {
        cerr << "Couldnt open file!\n";
        system("pause");
        return {};
    }
    string state;
    getline(in, state);
    string output(state.length(), '0');

    bool empty = 1;
    for (short i = 0; i < state.length(); i++) {
        if (state[i] != previous[i]) {
            empty = 0;
            if (state[i] == '1') output[i] = '1';
            else output[i] = '2';
        }
    }

    previous = state;
    if (empty) return {};
    return output;
}