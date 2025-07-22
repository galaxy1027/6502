#include "cpu6502.hpp"
#include "iostream"

int main(int argc, char **argv)
{
    cpu6502 *cpu = new cpu6502();
    std::cout << "Initialized...\n";
    std::cout << "Program loaded!\n";

    u8 opcode = cpu->Fetch();
    std::cout << "Opcode: 0x" << std::hex << (unsigned int)opcode << "\n";
    cpu->Execute(opcode);

    delete cpu;
    return 0;
}
