#pragma once

#include <common/base_state.h>

namespace assembler {

struct AssemblerState: public BaseState {
    std::unordered_map<std::string, std::vector<int>> label_args;
    std::unordered_map<std::string, int> labels;
};

} // namespace assembler
