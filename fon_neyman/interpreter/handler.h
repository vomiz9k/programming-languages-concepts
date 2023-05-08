#pragma once

#include <fstream>
#include <iostream>

#include <common/base_handler.h>

namespace interpreter {

class InterpreterHandler: public BaseHandler {
public:
    virtual void process(std::unordered_map<std::string, std::string>& args) override;
};


} //namespace interpreter
