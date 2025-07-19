#include "cpu6502.hpp"

#include <iostream>

cpu6502::cpu6502() {
    ram = std::array<u8, MAX_MEM>();
    Reset();

    //TODO: Delete temp program
    ram[0xFFFC] = 0xA9;
    ram[0xFFFD] = 0xBA;
}

void cpu6502::Reset() {
    ram.fill(0);

}

void cpu6502::Execute(u8 opcode) {
    opcode = Fetch();

    switch(opcode) {
        case 0xA9: { // LDA_IM
            u8 data = Fetch();
            A = data;
            Z = (A == 0);
            N = (A & (0x01 << 7)) != 0; // Check if bit 7 (sign bit) is set
        } break;

        default: {
            std::cerr << "ERROR: Unrecognized opcode\n";
        } break;
    }
}

u8 cpu6502::Fetch() {
    u8 data = ram[PC];
    PC++;
    cycles--;
    return data;
}
