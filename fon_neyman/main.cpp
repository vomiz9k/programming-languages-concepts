#include <assembler/handler.h>
#include <interpreter/handler.h>
#include <disassembler/handler.h>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    int i = 0;
    std::unordered_map<std::string, std::string> result;
    while (i < argc) {
        auto curr_arg = std::string(argv[i++]);
        if (curr_arg.substr(0, 2) == "--") {
            result[curr_arg.substr(2)] = argv[i++];
        } else {
            result[curr_arg.substr(1)] = "";
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    assembler::AssemblerHandler asm_handler;
    interpreter::InterpreterHandler interpreter_handler;
    disassembler::DisassemblerHandler disassembler_handler;

    auto args = parseArgs(argc - 1, argv + 1);

    std::vector<BaseHandler*> handlers;
    if (args.count("asm")) {
        handlers.push_back(&asm_handler);
    }
    if (args.count("interpret")) {
        handlers.push_back(&interpreter_handler);
    }
    if (args.count("disasm")) {
        handlers.push_back(&disassembler_handler);
    }

    for (const auto& handler: handlers) {
        handler->process(args);
    }
}