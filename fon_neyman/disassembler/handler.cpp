#include "handler.h"
#include "state.h"
#include <common/config.h>

namespace disassembler {

namespace {

void printArg(DisassemblerState& state, std::ofstream& output) {
    auto arg_type = ReversedConfig::arg_types.at(state.readByte());
    if (arg_type == "register") {
        output << ReversedConfig::registers.at(state.readByte());
    } else if (arg_type == "address") {
        const auto dim = state.readByte();
        if (dim == 1) {
            output << "chr";
        } else if (dim == 4) {
            output << "int";
        }
        output << '[';
        printArg(state, output);
        output << ']';
    } else if (arg_type == "label") {
        auto addr = state.readNumber();
        auto tmp = state.currPosition();
        state.currPosition() = addr + 1;
        output << ':' << state.readString();
        state.currPosition() = tmp;
    } else if (arg_type == "number") {
        output << state.readNumber();
    } else if (arg_type == "string") {
        output << '\"' << state.readString() << '\"';
    } else if (arg_type == "char") {
        output << '\'' << state.readByte() << '\'';
    }
}

void printLine(DisassemblerState& state, std::ofstream& output) {
    auto command = ReversedConfig::commands.at(state.readByte());
    if (command == "_label") {
        output << state.readString() << ':' << '\n';
    } else {
        output << command;
        for (std::size_t i = 0; i < Config::num_args.at(command); ++i) {
            output << ' ';
            printArg(state, output);
            if (i < Config::num_args.at(command) - 1) {
                output << ',';
            }
        }
        output << '\n';
    }
}

} // anonymous namespace

void DisassemblerHandler::process(std::unordered_map<std::string, std::string>& args) {
    std::ifstream input(args.at("disasm_input"));
    auto output_filename = args.count("disasm_output") > 0 ? args.at("disasm_output") : "disasm_output";
    std::ofstream output(output_filename, std::ios::binary | std::ios::out);

    DisassemblerState state(input);
    while (state.currPosition() < state.code.size()) {
        printLine(state, output);
    }
}

} // namespace disassembler
