#include "cpu6502.hpp"

#include <iostream>

cpu6502::cpu6502()
{
    ram = std::array<u8, MAX_MEM>();
    Reset();

    // TODO: Delete temp program
    ram[0xFFFC] = 0xA5;
    ram[0xFFFD] = 0x21;
    ram[0x0021] = 0x69;
}

/*
    Reset the 6502 CPU

    The Program Counter starts execution at the reset vector (located at 0xFFFC).
    The stack pointer is reset to the bottom of stack memory (0x0100 - 0x01FF).
    It contains the lower address byte, ranging from 0x00-0xFF.
*/
void cpu6502::Reset()
{
    ram.fill(0);
    PC = 0xFFFC;
    SP = 0xFF;
}

void cpu6502::Clock()
{
    if (cycles == 0)
    {
        u8 opcode = Fetch();
        std::cout << std::hex << (unsigned int)opcode << "\n"; // TODO: delete line
        Execute(opcode);
    }
    cycles--;
}

u8 cpu6502::Fetch()
{
    u8 data = ram[PC];
    PC++;
    cycles--;
    return data;
}

void cpu6502::Execute(u8 opcode)
{
    switch (opcode)
    {
    case 0x20: // JSR
    {
        // Obtain the address to jump to by fetching 2 bytes
        u16 jumpAddr = 0x0000;
        jumpAddr |= Fetch();
        jumpAddr |= (Fetch() << 8);

        // Push the 16 bit PC address onto the stack
        u16 returnAddr = PC - 1;
        ram[0x0100 + SP--] = (returnAddr & 0x00FF);
        ram[0x0100 + SP--] = (returnAddr >> 8) & 0x00FF;

        PC = jumpAddr;
        cycles += 6;
    }
    break;
    case 0xA5: // LDA_ZP
    {
        u8 zeroPageAddr = Fetch();
        u8 data = ram[zeroPageAddr];
        LDA(data);
        cycles += 3;
    }
    break;
    case 0xA9: // LDA_IM
    {
        u8 data = Fetch();
        LDA(data);
        cycles += 2;
    }
    break;
    case 0xB5: // LDA_ZPX
    {
        u8 zeroPageAddr = Fetch() + X;
        u8 data = ram[zeroPageAddr];
        LDA(data);
        cycles += 4;
    }
    break;
    default:
    {
        std::cerr << "ERROR: Unrecognized opcode\n";
        std::exit(EXIT_FAILURE);
    }
    break;
    }
}

void cpu6502::LDA(u8 data)
{
    A = data;
    Z = (A == 0);
    N = (A & (0x01 << 7)) != 0; // Check if bit 7 (sign bit) is set
}