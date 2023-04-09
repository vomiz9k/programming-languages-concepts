#pragma once

#include <fstream>

#include <common/base_handler.h>

namespace disassembler {

class DisassemblerHandler: public BaseHandler {
public:
    virtual void process(std::unordered_map<std::string, std::string>& args) override;
};

} // namespace disassembler
