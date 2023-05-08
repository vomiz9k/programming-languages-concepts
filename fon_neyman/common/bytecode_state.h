#pragma once

#include "base_state.h"
#include <string>
#include <fstream>

struct BytecodeState: public BaseState {
    BytecodeState(std::ifstream& input);
    char readByte();
    std::string readString();
    int readNumber();
    virtual int& currPosition() = 0;
};