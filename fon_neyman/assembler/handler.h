#pragma once

#include <vector>

#include <common/base_handler.h>

namespace assembler {

class AssemblerHandler: public BaseHandler {
public:
    virtual void process(std::unordered_map<std::string, std::string>& args) override;
};


} //namespace asm
