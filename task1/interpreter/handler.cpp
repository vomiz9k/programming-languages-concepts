#include "handler.h"
#include "state.h"
#include <common/config.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <charconv>
#include <stack>
#include <unordered_map>
#include <array>

namespace interpreter {

namespace {

void processPush(InterpreterState& state) {
    state.stack.push(state.getNextNumArg());
}

void processPop(InterpreterState& state) {
    state.putToNextArg(state.stack.top());
    state.stack.pop();
}

#define processBinOp(state, OP) {                                                      \
    auto arg1_type = ReversedConfig::arg_types.at(state.readByte());                   \
    assert(arg1_type == "register" || arg1_type == "address");                         \
                                                                                       \
    if (arg1_type == "register") {                                                     \
        int& val = state.registers[state.readByte()];                                  \
        val OP state.getNextNumArg();                                                  \
    } else if (arg1_type == "address") {                                               \
        auto dim = state.readByte();                                                   \
        auto addr = state.getNextNumArg();                                             \
        if (dim == 1) {                                                                \
            state.memory[addr] OP state.getNextNumArg();                               \
        } else if (dim == 4) {                                                         \
            *(int*)(&state.memory[addr]) OP state.getNextNumArg();                     \
        }                                                                              \
    }                                                                                  \
}

void processJmp(InterpreterState& state) {
    assert (Config::arg_types.at("label") == state.readByte());
    state.currPosition() = state.readNumber();
}

void processCall(InterpreterState& state) {
    assert (Config::arg_types.at("label") == state.readByte());
    int next_position = state.readNumber();
    state.stack.push(state.currPosition());
    state.currPosition() = next_position;
}

void processRet(InterpreterState& state) {
    auto return_addr = state.stack.top();
    state.stack.pop();
    state.currPosition() = return_addr;
}

void processCmp(InterpreterState& state) {
    auto arg1 = state.getNextNumArg();
    auto arg2 = state.getNextNumArg();

    state.stack.push(arg1 - arg2);
}

void processJe(InterpreterState& state) {
    assert (Config::arg_types.at("label") == state.readByte());

    auto value = state.stack.top();
    state.stack.pop();

    if (value == 0) {
        state.currPosition() = state.readNumber();
    } else {
        state.readNumber();
    }
}

void processLabel(InterpreterState& state) {
    state.readString();
}

void processJg(InterpreterState& state) {
    assert (Config::arg_types.at("label") == state.readByte());

    auto value = state.stack.top();
    state.stack.pop();

    if (value > 0) {
        state.currPosition() = state.readNumber();
    } else {
        state.readNumber();
    }
}

void processIn(InterpreterState& state) {
    int num;
    std::cin >> num;

    state.putToNextArg(num);
}

void processOut(InterpreterState& state) {
    auto arg_type = ReversedConfig::arg_types.at(state.readByte());
    assert(arg_type != "label");

    if (arg_type == "char") {
        std::cout << state.readByte();
    } else {
        state.currPosition()--;
        std::cout << (char)state.getNextNumArg();
    }
}

void processCommand(InterpreterState& state) {
    auto command = ReversedConfig::commands.at(state.readByte());

    if (command == "push") {
        processPush(state);
    } else if (command == "pop") {
        processPop(state);
    } else if (command == "add") {
        processBinOp(state, +=);
    } else if (command == "sub") {
        processBinOp(state, -=);
    } else if (command == "mul") {
        processBinOp(state, *=);
    } else if (command == "div") {
        processBinOp(state, /=);
    } else if (command == "mov") {
        processBinOp(state, =);
    } else if (command == "call") {
        processCall(state);
    } else if (command == "ret") {
        processRet(state);
    } else if (command == "jmp") {
        processJmp(state);
    } else if (command == "je") {
        processJe(state);
    } else if (command == "jg") {
        processJg(state);
    } else if (command == "cmp") {
        processCmp(state);
    } else if (command == "in") {
        processIn(state);
    } else if (command == "out") {
        processOut(state);
    } else if (command == "dump") {
        state.dump();
    } else if (command == "_label") {
        processLabel(state);
    }

}

} // anonymous namespace

void InterpreterHandler::process(std::unordered_map<std::string, std::string>& args) {
    std::ifstream input(args.at("interpreter_input"));
    InterpreterState state(input);

    while (state.currPosition() < state.code.size()) {
        processCommand(state);
    }
}

} //namespace interpreter