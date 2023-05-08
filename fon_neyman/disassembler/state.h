#include <common/bytecode_state.h>

namespace disassembler {

struct DisassemblerState: public BytecodeState {
    DisassemblerState(std::ifstream& input) : BytecodeState(input) {};
    int curr_position = 0;
    virtual int& currPosition() override { return curr_position; }
};

} // namespace disassembler
