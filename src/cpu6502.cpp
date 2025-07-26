#include "cpu6502.hpp"

#include "bus.hpp"
#include <iostream>

cpu6502::cpu6502()
{
}

/*
    Reset the 6502 CPU

    The Program Counter starts execution at the reset vector (located at
    0xFFFC). The stack pointer is reset to the bottom of stack memory (0x0100 -
    0x01FF). It contains the lower address byte, ranging from 0x00-0xFF.
*/
void cpu6502::Init()
{
    PC = bus->cpuRead(0xFFFC) | (bus->cpuRead(0xFFFD) << 8);
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
    u8 data = bus->cpuRead(PC);
    PC++;
    return data;
}

u16 cpu6502::FetchWord()
{
    u16 data = Fetch() | (Fetch() << 8);
    return data;
}

void cpu6502::StoreByte(u8 val, u16 addr)
{
    bus->cpuWrite(val, addr);
}

void cpu6502::ConnectBus(Bus *b)
{
    bus = b;
}
void cpu6502::Execute(u8 opcode)
{
    std::cout << "0x" << std::hex << PC << ":\t" << std::hex << (unsigned int)opcode << "\n"; // TODO: delete line
    switch (opcode)
    {
    case 0x00: // BRK
    {
        PushStackWord(PC + 1);
        PushStackFlags();
        B = 1;
        PC = (bus->cpuRead(0xFFFE)) | (bus->cpuRead(0xFFFF) << 8); // Load the IRQ vector into the PC
        cycles += 7;
    }
    break;

    case 0x10: // BPL
    {
        i8 displacement = Fetch();
        if (N == 0)
        {
            u16 oldPC = PC; // Save the old PC location for checking page crossing
            PC += displacement;
            cycles += 1;

            if ((oldPC & 0xFF00) != (PC & 0xFF00)) // Branched to different mem page
                cycles += 1;
        }
        cycles += 2;
    }
    break;
    case 0x18: // CLC
    {
        C = 0;
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
        PushStackWord(returnAddr);
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
    case 0x40: // RTI
    {
        PopStackFlags();
        PC = PopStackWord();
        cycles += 6;
    }
    break;
    case 0x48: // PHA
    {
        PushStack(A);
        cycles += 3;
    }
    break;
    case 0x4C: // JMP_ABS
    {
        PC = FetchWord();
        cycles += 3;
    }
    break;
    case 0x60: // RTS
    {
        PC = PopStackWord() + 1;
        cycles += 6;
    }
    break;
    case 0x69: // ADC_IM
    {
        u8 data = Fetch();
        ADD(data);
        cycles += 2;
    }
    break;
    case 0x6C: // JMP_IND
    {
        u16 pointer = FetchWord();
        u8 addrLSB = bus->cpuRead(pointer);

        // 6502 original hardware bug: page fails to wrap if the lower byte is 0xFF
        // For example, JMP $03FF reads 03FF + 0300 instead of 03FF + 0400
        u8 addrMSB;
        if ((pointer & 0x00FF) == 0xFF)
            addrMSB = bus->cpuRead(pointer & 0xFF00);
        else
            addrMSB = bus->cpuRead(pointer + 1);

        PC = addrLSB | (addrMSB << 8);
        cycles += 5;
    }
    break;
    case 0x78: // SEI
    {
        I = 1;
        cycles += 2;
    }
    break;
    case 0x84: // STY_ZP
    {
        u8 zeroPageAddr = Fetch();
        STR(zeroPageAddr, REG_Y);
        cycles += 3;
    }
    break;
    case 0x85: // STA_ZP
    {
        u8 zeroPageAddr = Fetch();
        STR(zeroPageAddr, REG_A);
        cycles += 3;
    }
    break;
    case 0x86: // STX_ZP
    {
        u8 zeroPageAddr = Fetch();
        STR(zeroPageAddr, REG_X);
        cycles += 3;
    }
    break;
    case 0x88: // DEY
    {
        DEC_REG(REG_Y);
        cycles += 2;
    }
    break;
    case 0x8A: // TXA
    {
        TransferReg(&X, &A);
        cycles += 2;
    }
    break;
    case 0x8C: // STY_ABS
    {
        u16 addr = FetchWord();
        STR(addr, REG_Y);
        cycles += 4;
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
    case 0x90: // BCC
    {
        i8 displacement = Fetch();
        if (C == 0)
        {
            u16 oldPC = PC;
            PC += displacement;
            cycles += 1;

            if ((oldPC & 0xFF00) != (PC & 0xFF00)) // Branched to different mem page
                cycles += 1;
        }
        cycles += 2;
    }
    break;
    case 0x98: // TYA
    {
        TransferReg(&Y, &A);
        cycles += 2;
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
    case 0xA0: // LDY_IM
    {
        u8 data = Fetch();
        LD(data, REG_Y);
        cycles += 2;
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
        u8 data = bus->cpuRead(zeroPageAddr);
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
        u8 data = bus->cpuRead(addr);
        LD(data, REG_A);
        cycles += 4;
    }
    break;
    case 0xB5: // LDA_ZPX
    {
        u8 zeroPageAddr = Fetch() + X;
        u8 data = bus->cpuRead(zeroPageAddr);
        LD(data, REG_A);
        cycles += 4;
    }
    break;
    case 0xB9: // LDA_ABS_Y
    {
        u16 baseAddr = FetchWord();
        u16 absAddr = baseAddr + Y;
        u8 data = bus->cpuRead(absAddr);
        LD(data, REG_A);

        if ((baseAddr & 0xFF00) != (absAddr & 0xFF00)) // Check if page was crossed
            cycles += 1;

        cycles += 4;
    }
    break;
    case 0xBD: // LDA_ABS_X
    {
        u16 baseAddr = FetchWord();
        u16 absAddr = baseAddr + X;
        u8 data = bus->cpuRead(absAddr);
        LD(data, REG_A);

        if ((baseAddr & 0xFF00) != (absAddr & 0xFF00)) // Check if page was crossed
            cycles += 1;

        cycles += 4;
    }
    break;
    case 0xC0: // CPY_IM
    {
        u8 data = Fetch();
        CMP(data, REG_Y);
        cycles += 2;
    }
    break;
    case 0xC6: // DEC_ZP
    {
        u16 addr = Fetch();
        DEC_MEM(addr);
        cycles += 5;
    }
    break;
    case 0xC8: // INY
    {
        INC_REG(REG_Y);
        cycles += 2;
    }
    break;
    case 0xCA: // DEX
    {
        DEC_REG(REG_X);
        cycles += 2;
    }
    break;
    case 0xD0: // BNE
    {
        i8 displacement = Fetch();
        if (Z == 0)
        {
            u16 oldPC = PC;
            PC += displacement;
            cycles += 1;

            if ((oldPC & 0xFF00) != (PC & 0xFF00)) // Branched to different mem page
                cycles += 1;
        }
        cycles += 2;
    }
    break;
    case 0xD8: // CLD
    {
        D = 0;
        cycles += 2;
    }
    break;
    case 0xE0: // CPX_IM
    {
        u8 data = Fetch();
        CMP(data, REG_X);
        cycles += 2;
    }
    break;
    case 0xE6: // INC_ZP
    {
        u16 addr = Fetch();
        INC_MEM(addr);
        cycles += 5;
    }
    break;
    case 0xE8: // INX
    {
        INC_REG(REG_X);
        cycles += 2;
    }
    break;
    case 0xEA: // NOP
    {
        cycles += 2;
    }
    break;
    case 0xF0: // BEQ
    {
        i8 displacement = Fetch();
        if (Z == 1)
        {
            u16 oldPC = PC;
            PC += displacement;
            cycles += 1;

            if ((oldPC & 0xFF00) != (PC & 0xFF00)) // Branched to different mem page
                cycles += 1;
        }
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

void cpu6502::PushStack(u8 data)
{
    bus->cpuWrite(data, 0x0100 + SP);
    SP--;
}

void cpu6502::PushStackWord(u16 data)
{
    u8 dataMSB = (data >> 8) & 0x00FF;
    u8 dataLSB = (data) & 0x00FF;
    PushStack(dataMSB);
    PushStack(dataLSB);
}

void cpu6502::PushStackFlags()
{
    u8 flags = (C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (1 << 5) | (V << 6) | (N << 7);
    PushStack(flags);
}

u8 cpu6502::PopStack()
{
    SP++;
    return bus->cpuRead(0x0100 + SP);
}

u16 cpu6502::PopStackWord()
{
    u8 dataLSB = PopStack();
    u8 dataMSB = PopStack();
    u16 data = (dataMSB << 8) | (dataLSB);
    return data;
}

void cpu6502::PopStackFlags()
{
    u8 flags = PopStack();
    C = (flags >> 0) & 0x1;
    Z = (flags >> 1) & 0x1;
    I = (flags >> 2) & 0x1;
    D = (flags >> 3) & 0x1;
    B = (flags >> 4) & 0x1;
    V = (flags >> 6) & 0x1;
    N = (flags >> 7) & 0x1;
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

    bus->cpuWrite(data, addr);
}

void cpu6502::INC_REG(enum reg r)
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

void cpu6502::INC_MEM(u16 addr)
{
    u8 data = bus->cpuRead(addr);
    data++;
    Z = (data == 0);
    N = (data & 0b10000000) != 0;
    bus->cpuWrite(data, addr);
}

void cpu6502::DEC_REG(enum reg r)
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

void cpu6502::DEC_MEM(u16 addr)
{
    u8 result = bus->cpuRead(addr) - 1;
    bus->cpuWrite(result, addr);
    Z = (result == 0);
    N = (result & 0b10000000) != 0;
}

void cpu6502::AND(u8 data)
{
    A &= data;
    Z = (A == 0);
    N = (A & 0b10000000) != 0;
}

void cpu6502::CMP(u8 data, enum reg r)
{
    u8 regVal;
    if (r == REG_A)
        regVal = A;
    else if (r == REG_X)
        regVal = X;
    else if (r == REG_Y)
        regVal = Y;

    u8 result = regVal - data;
    C = (regVal >= data);
    Z = (regVal == data);
    N = (result & 0b10000000) != 0;
}

void cpu6502::TransferReg(u8 *src, u8 *dest)
{
    *dest = *src;

    Z = (*dest == 0);
    N = (*dest & 0b10000000) != 0;
}

void cpu6502::ADD(u8 data)
{
    u16 sum = A + data + C;

    V = (~(A ^ data) & (A ^ sum) & 0x80) != 0;
    C = (sum > 0xFF);

    A = (sum & 0x00FF);

    Z = (A == 0);
    N = (A & 0b10000000) != 0;
}