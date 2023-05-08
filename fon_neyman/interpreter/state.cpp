#include "state.h"

#include <iostream>
#include <common/config.h>

namespace interpreter {

InterpreterState::InterpreterState(std::ifstream& input) : BytecodeState(input) {
    for (const auto& [register_str, register_byte] : Config::registers) {
        registers[register_byte] = 0;
    }
}

int InterpreterState::getNextNumArg() {
    auto arg_type = ReversedConfig::arg_types.at(readByte());
    assert(arg_type != "label" && arg_type != "string");

    if (arg_type == "register") {
        return registers.at(readByte());
    } else if (arg_type == "number") {
        return readNumber();
    } else if (arg_type == "address") {
        auto dim = readByte();
        auto addr = getNextNumArg();
        if (dim == 1) {
            return memory[addr];
        } else if (dim == 4) {
            return *(int*)(&memory[addr]);
        }
    } else if (arg_type == "char") {
        return readByte();
    }
    return 0;
}

void InterpreterState::putToNextArg(int value) {
    auto arg_type = ReversedConfig::arg_types.at(readByte());
    assert(arg_type == "register" || arg_type == "address");

    if (arg_type == "register") {
        registers[readByte()] = value;
    }
    else if (arg_type == "address") {
        auto dim = readByte();
        auto addr = getNextNumArg();

        if (dim == 1) {
            memory[addr] = value;
        } else if (dim == 4) {
            *(int*)(&memory[addr]) = value;
        }
    }
}

int& InterpreterState::currPosition() {
    return registers.at(Config::registers.at("rip"));
}

void InterpreterState::dump() {
    std::cout << "---DUMPING InterpreterState---\n";
    std::cout << "Registers:";
    for (const auto& [reg, val]: registers) {
        std::cout << '\t' << ReversedConfig::registers.at(reg) << ": " << val << '\n';
    }
    std::cout << "-------------------\n";
}

} // namespace interpreter