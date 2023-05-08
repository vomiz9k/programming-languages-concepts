#pragma once

#include <string>
#include <unordered_map>

class BaseHandler {
public:
    virtual void process(std::unordered_map<std::string, std::string>& args) = 0;
    virtual ~BaseHandler() = default;
};