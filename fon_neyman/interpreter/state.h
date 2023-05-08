#pragma once

#include <fstream>
#include <unordered_map>
#include <stack>
#include <array>

#include <common/bytecode_state.h>

namespace interpreter {

struct InterpreterState : public BytecodeState {
    InterpreterState(std::ifstream& input);
    std::unordered_map<char, int> registers;
    std::array<char, 10000> memory;
    std::stack<int> stack;

    int getNextNumArg();
    void putToNextArg(int value);
    void dump();
    virtual int& currPosition() override;
};

} // namespace interpreter