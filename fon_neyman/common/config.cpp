#include "config.h"

const std::unordered_map<std::string_view, char> Config::commands = {
    {"push",   0x01},
    {"pop",    0x02},
    {"add",    0x11},
    {"sub",    0x12},
    {"mul",    0x13},
    {"div",    0x14},
    {"mov",    0x15},
    {"call",   0x21},
    {"ret",    0x22},
    {"jmp",    0x23},
    {"je",     0x24},
    {"jg",     0x26},
    {"cmp",    0x2a},
    {"in",     0xa1},
    {"out",    0xa2},
    {"dump",   0xfe},
    {"_label", 0xff}
};

const std::unordered_map<std::string_view, char> Config::num_args = {
    {"push",   1},
    {"pop",    1},
    {"add",    2},
    {"sub",    2},
    {"mul",    2},
    {"div",    2},
    {"mov",    2},
    {"call",   1},
    {"ret",    0},
    {"jmp",    1},
    {"je",     1},
    {"jg",     1},
    {"cmp",    2},
    {"in",     1},
    {"out",    1},
    {"dump",   0},
    {"_label", 1}
};

const std::unordered_map<std::string_view, char> Config::arg_types = {
    {"register", 0x01},
    {"address",  0x02},
    {"label",    0x03},
    {"number",   0x04},
    {"string",   0x05},
    {"char",     0x06}
};

const std::unordered_map<std::string_view, char> Config::registers = {
    {"rax", 0x0a},
    {"rbx", 0x0b},
    {"rcx", 0x0c},
    {"rdx", 0x0d},
    {"rsp", 0x11},
    {"rbp", 0x12},
    {"rsi", 0x13},
    {"rdi", 0x14},
    {"rip", 0x21}
};

std::unordered_map<char, std::string_view> reverseMap(const std::unordered_map<std::string_view, char> map) {
    std::unordered_map<char, std::string_view> result;
    for (const auto& [k, v]: map) {
        result[v] = k;
    }
    return result;
}

const std::unordered_map<char, std::string_view> ReversedConfig::commands = reverseMap(Config::commands);
const std::unordered_map<char, std::string_view> ReversedConfig::arg_types = reverseMap(Config::arg_types);
const std::unordered_map<char, std::string_view> ReversedConfig::registers = reverseMap(Config::registers);

