#include "bytecode_state.h"

BytecodeState::BytecodeState(std::ifstream& input) {
    input.seekg(0, std::ios::end);
    std::size_t length = input.tellg();
    input.seekg(0, std::ios::beg);

    code.resize(length);
    input.read(code.data(), length);
}

char BytecodeState::readByte() {
    return code[currPosition()++];
}

std::string BytecodeState::readString() {
    std::string result;
    while (code[currPosition()] != 0) {
        result.push_back(code[currPosition()]);
        ++currPosition();
    }
    ++currPosition();
    return result;
}

int BytecodeState::readNumber() {
    int result = *(int*)(&code[currPosition()]);
    currPosition() += 4;
    return result;
}
