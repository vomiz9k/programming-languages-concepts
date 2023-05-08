#include "handler.h"
#include "state.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <charconv>
#include <common/config.h>
#include <common/base_state.h>



namespace assembler {

namespace {

bool isNumber(std::string_view s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool isRegister(std::string_view s) {
    return Config::registers.count(s) > 0;
}

bool isAddress(std::string_view s) {
    if (s.size() < 5 || s[s.size() - 1] != ']') {
        return false;
    }
    if (s.substr(0, 4) == "int[" || s.substr(0, 4) == "chr[") {
        return isNumber(s.substr(4, s.size() - 5)) || isRegister(s.substr(4, s.size() - 5)) || isAddress(s.substr(4, s.size() - 5));
    }
    return false;
}

bool isValidIdentifier(std::string_view s) {
    return !isRegister(s) && (isalpha(s[0]) || s[0] == '_') && std::all_of(s.begin() + 1, s.end(), [](const char ch) {
        return isalpha(ch) || isdigit(ch) || ch == '_';
    });
}
bool isLabelArg(std::string_view s) {
    return s.size() > 2 && s[0] == ':' && isValidIdentifier(s.substr(1));
}

bool isString(std::string_view s) {
    return s.size() > 2 && s[0] == '\"' && s[s.size() - 1] == '\"';
}

bool isEmpty(std::string_view s) {
    return s.empty() || s.find_first_not_of(" \t") == std::string_view::npos;
}

bool isLabel(std::string_view s) {
    return s.size() > 2 && s[s.size() - 1] == ':' && isValidIdentifier(s.substr(0, s.size() - 1));
}

bool isChar(std::string_view s) {
    return s.size() == 3 && s[0] == '\'' && s[2] == '\'';
}

void processArg(std::string_view curr_arg, AssemblerState& state) {
    if (isRegister(curr_arg)) {
        state.code.push_back(Config::arg_types.at("register"));
        state.code.push_back(Config::registers.at(curr_arg));
    } else if (isNumber(curr_arg)) {
        state.code.push_back(Config::arg_types.at("number"));
        int num;
        std::from_chars(curr_arg.data(), curr_arg.data() + curr_arg.size(), num);
        char* num_ptr = (char*)&num;
        state.code.insert(state.code.end(), num_ptr, num_ptr + sizeof(num));
    } else if (isLabelArg(curr_arg)) {
        state.code.push_back(Config::arg_types.at("label"));
        state.label_args[std::string(curr_arg.substr(1))].push_back(state.code.size());
        for(int i = 0; i < 4; ++i) {
            state.code.push_back(0);
        }
    } else if (isAddress(curr_arg)) {
        state.code.push_back(Config::arg_types.at("address"));
        if (curr_arg.substr(0, 3) == "int") {
            state.code.push_back(4);
        } else if (curr_arg.substr(0, 3) == "chr") {
            state.code.push_back(1);
        }
        curr_arg.remove_prefix(4);
        curr_arg.remove_suffix(1);
        processArg(curr_arg, state);
    } else if (isString(curr_arg)) {
        state.code.push_back(Config::arg_types.at("string"));
        state.code.insert(state.code.end(), curr_arg.begin() + 1, curr_arg.end() - 1);
        state.code.push_back(0);
    } else if (isChar(curr_arg)) {
        state.code.push_back(Config::arg_types.at("char"));
        state.code.push_back(curr_arg[1]);
    }
}

void processLine(std::string_view line, AssemblerState& state) {
    if (isEmpty(line)) {
        return;
    }

    const auto command_end = line.find(' ');
    const auto command = line.substr(0, command_end);
    // std::cout << command << '\n';
    if (isLabel(command)) {
        state.labels[std::string(command.substr(0, command.size() - 1))] = state.code.size();
        state.code.push_back(Config::commands.at("_label"));

        state.code.insert(state.code.end(), command.begin(), command.end() - 1);
        state.code.push_back(0);
        return;
    }

    const auto command_byte = Config::commands.at(command);
    state.code.push_back(command_byte);

    line.remove_prefix(command_end);
    line.remove_prefix(line.find_first_not_of(" ,\t"));

    while (!line.empty()) {
        std::size_t curr_arg_end = 0;
        if (line[0] == '\'' && (curr_arg_end = line.substr(1).find('\'') != std::string_view::npos)) {
            curr_arg_end = line.substr(1).find('\'') + 2;
        } else if (line[0] == '\"' && (curr_arg_end = line.substr(1).find('\"') != std::string_view::npos)) {
            curr_arg_end = line.substr(1).find('\"') + 2;
        } else {
            curr_arg_end = line.find_first_of(" ,\t");
        }

        auto curr_arg = line.substr(0, curr_arg_end);
        processArg(curr_arg, state);

        if (curr_arg_end == std::string_view::npos || curr_arg_end == line.size()) {
            break;
        }

        line.remove_prefix(curr_arg_end);
        line.remove_prefix(line.find_first_not_of(" ,\t"));
    }
}

void putLabels(AssemblerState& state) {
    for (const auto& [label, position]: state.labels) {
        for (const auto label_arg_position: state.label_args[label]) {
            *(int*)(&state.code[label_arg_position]) = position;
        }
    }
}

} // anonymous namespace

void AssemblerHandler::process(std::unordered_map<std::string, std::string>& args) {
    std::ifstream input(args.at("asm_input"));
    auto output_filename = args.count("asm_output") > 0 ? args.at("asm_output") : "asm_output";
    std::ofstream output(output_filename, std::ios::binary | std::ios::out);

    args["interpreter_input"] = output_filename;
    args["disasm_input"] = output_filename;

    std::string line;
    AssemblerState state;
    while (std::getline(input, line)) {
        processLine(line, state);
    }
    putLabels(state);

    output.write(state.code.data(), state.code.size());
}

} // namespace asm
