#include "cpu6502.hpp"

#include <iostream>

cpu6502::cpu6502()
{
    ram = std::array<u8, MAX_MEM>();
    ram.fill(0);
}

/*
    Reset the 6502 CPU

    The Program Counter starts execution at the reset vector (located at
    0xFFFC). The stack pointer is reset to the bottom of stack memory (0x0100 -
    0x01FF). It contains the lower address byte, ranging from 0x00-0xFF.
*/
void cpu6502::Init()
{
    PC = ram[0xFFFC] | (ram[0xFFFD] << 8);
    SP = 0xFF;
}

void cpu6502::Clock()
{
    if (cycles == 0)
    {
        u8 opcode = Fetch();
        Execute(opcode);
    }
    cycles--;
}

u8 cpu6502::Fetch()
{
    u8 data = ram[PC];
    PC++;
    return data;
}

u8 cpu6502::Read(u16 addr)
{
    u8 data;
    switch (addr)
    {
    case 0x2002:
        data = 0x80;
        break;
    default:
        data = ram[addr];
        break;
    }
    return data;
}

u16 cpu6502::FetchWord()
{
    u16 data = Fetch() | (Fetch() << 8);
    return data;
}

void cpu6502::StoreByte(u8 val, u16 addr)
{
    ram[addr] = val;
}

void cpu6502::Execute(u8 opcode)
{
    std::cout << std::hex << (unsigned int)opcode << "\n"; // TODO: delete line
    switch (opcode)
    {
    case 0x00: // BRK
    {
        PushStack(PC);
        PushStackFlags();
        B = 1;
        PC = (ram[0xFFFE]) | (ram[0xFFFF] << 8); // Load the IRQ vector into the PC
        cycles += 7;
    }
    break;

    case 0x10: // BPL
    {
        i8 relativeOffset = Fetch();
        if (N == 0)
        {
            std::cout << "branch offset = " << (int)relativeOffset << std::endl;
            u16 oldPC = PC; // Save the old PC location for checking page crossing
            PC += relativeOffset;
            cycles += 1;

            if ((oldPC & 0xFF00) != (PC & 0xFF00)) // Branched to different mem page
                cycles += 1;
        }
        cycles += 2;
    }
    break;
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
    case 0x29: // AND_IM
    {
        u8 data = Fetch();
        A &= data;
        cycles += 2;
    }
    break;
    case 0x48: // PHA
    {
        PushStack(A);
        cycles += 3;
    }
    break;
    case 0x4E: // LSR_ABS
    {
        u16 addr = FetchWord();
        u8 *dataAddr = &ram[addr];
        LSR(dataAddr);
        cycles += 6;
    }
    break;
    case 0x78: // SEI
    {
        I = 1;
        cycles += 2;
    }
    break;
    case 0x85: // STA_ZP
    {
        u8 zeroPageAddr = Fetch();
        STR(zeroPageAddr, REG_A);
        cycles += 3;
    }
    break;
    case 0x8D: // STA_ABS
    {
        u16 addr = FetchWord();
        STR(addr, REG_A);
        cycles += 4;
    }
    break;
    case 0x8E: // STX_ABS
    {
        u16 addr = FetchWord();
        STR(addr, REG_X);
        cycles += 4;
    }
    break;
    case 0x99: // STA_ABS_Y
    {
        u16 addr = FetchWord() + Y;
        STR(addr, REG_A);
        cycles += 5;
    }
    break;
    case 0x9A: // TXS
    {
        SP = X;
        cycles += 2;
    }
    break;
    case 0x9D: // STA_ABS_X
    {
        u16 addr = FetchWord() + X;
        STR(addr, REG_A);
        cycles += 5;
    }
    break;
    case 0xA2: // LDX_IM
    {
        u8 data = Fetch();
        LD(data, REG_X);
        cycles += 2;
    }
    break;
    case 0xA5: // LDA_ZP
    {
        u8 zeroPageAddr = Fetch();
        u8 data = ram[zeroPageAddr];
        LD(data, REG_A);
        cycles += 3;
    }
    break;
    case 0xA8: // TAY
    {
        Y = A;
        Z = (Y == 0);
        N = (Y & 0b10000000) != 0;
        cycles += 2;
    }
    break;
    case 0xA9: // LDA_IM
    {
        u8 data = Fetch();
        LD(data, REG_A);
        cycles += 2;
    }
    break;
    case 0xAA: // TAX
    {
        X = A;
        Z = (X == 0);
        N = (X & 0b10000000) != 0;
        cycles += 2;
    }
    break;
    case 0xAD: // LDA_ABS
    {
        u16 addr = Fetch() | (Fetch() << 8);
        u8 data = Read(addr);
        LD(data, REG_A);
        cycles += 4;
    }
    break;
    case 0xB5: // LDA_ZPX
    {
        u8 zeroPageAddr = Fetch() + X;
        u8 data = ram[zeroPageAddr];
        LD(data, REG_A);
        cycles += 4;
    }
    break;
    case 0xCA: // DEX
    {
        DEC(REG_X);
        cycles += 2;
    }
    break;
    case 0xD8: // CLD
    {
        D = 0;
        cycles += 2;
    }
    break;
    case 0xE8: // INX
    {
        INC(REG_X);
        cycles += 2;
    }
    break;
    case 0xEA: // NOP
    {
        cycles += 2;
    }
    break;
    default: {
        std::cerr << "ERROR: Unrecognized opcode 0x" << std::hex << (unsigned int)opcode << std::endl;
        std::exit(EXIT_FAILURE);
    }
    break;
    }
}

void cpu6502::PushStackFlags()
{
    u8 flags = (C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (1 << 5) | (V << 6) | (N << 7);
    ram[SP] = flags;
    SP--;
}

void cpu6502::PushStack(u8 data)
{
    ram[SP] = data;
    SP--;
}
void cpu6502::PushStackWord(u16 data)
{
    u8 dataMSB = (data >> 8) & 0x00FF;
    u8 dataLSB = (data) & 0x00FF;
    PushStack(dataLSB);
    PushStack(dataMSB);
    SP -= 2;
}

u8 cpu6502::PopStack()
{
    u8 data = ram[SP];
    SP++;
    return data;
}

u16 cpu6502::PopStackWord()
{
    u16 data = (ram[SP] << 8) | (ram[SP - 1]);
    SP += 2;
    return data;
}

void cpu6502::LD(u8 data, enum reg r)
{
    if (r == REG_A)
        A = data;
    else if (r == REG_X)
        X = data;
    else if (r == REG_Y)
        Y = data;

    Z = (data == 0);
    N = (data & 0b10000000) != 0; // Check if bit 7 (sign bit) is set
}

void cpu6502::LSR(u8 *dataAddr)
{
    C = *dataAddr & 0x01;
    *dataAddr >>= 1;
    Z = (*dataAddr == 0);
    N = 0;
}

void cpu6502::STR(u16 addr, enum reg r)
{
    u8 data;
    if (r == REG_A)
        data = A;
    else if (r == REG_X)
        data = X;
    else if (r == REG_Y)
        data = Y;

    ram[addr] = data;
}

void cpu6502::INC(enum reg r)
{
    u8 *reg;
    if (r == REG_A)
        reg = &A;
    else if (r == REG_X)
        reg = &X;
    else if (r == REG_Y)
        reg = &Y;
    (*reg)++;
    Z = (*reg == 0);
    N = (*reg & 0b10000000) != 0;
}

void cpu6502::DEC(enum reg r)
{
    u8 *reg;
    if (r == REG_A)
        reg = &A;
    else if (r == REG_X)
        reg = &X;
    else if (r == REG_Y)
        reg = &Y;
    (*reg)--;
    Z = (*reg == 0);
    N = (*reg & 0b10000000) != 0;
}
void cpu6502::AND(u8 data)
{
    A &= data;
    Z = (A == 0);
    N = (A & 0b10000000) != 0;
}