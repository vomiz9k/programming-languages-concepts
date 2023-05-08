#pragma once

#include <unordered_map>
#include <string_view>

struct Config {
    static const std::unordered_map<std::string_view, char> commands;
    static const std::unordered_map<std::string_view, char> arg_types;
    static const std::unordered_map<std::string_view, char> registers;
    static const std::unordered_map<std::string_view, char> num_args;
};

struct ReversedConfig {
    static const std::unordered_map<char, std::string_view> commands;
    static const std::unordered_map<char, std::string_view> arg_types;
    static const std::unordered_map<char, std::string_view> registers;
};
