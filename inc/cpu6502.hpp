#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>

#define MAX_MEM 64 * 1024

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

class cpu6502
{
  private:
    /* Register names enum */
    enum reg
    {
        REG_A,
        REG_X,
        REG_Y
    };
    /* Registers */
    u8 A, X, Y = 0x00; // General Registers
    u8 SP;             // Stack Pointer
    u16 PC;            // Program Counter

    /* Status flags */
    bool C; // Carry
    bool Z; // Zero
    bool I; // Interrupt disable
    bool D; // Decimal mode
    bool B; // Break
    bool V; // Overflow
    bool N; // Negative

    u8 opcode;
    u32 cycles; // Number of cycles left for the CPU to complete on current instruciton

    /* Memory */
  public:
    std::array<u8, MAX_MEM> ram;

    /* Private opcode functions */
  private:
    void LD(u8 data, enum reg r);
    void LSR(u8 *dataAddr);
    void STR(u16 addr, enum reg r);
    void INC_REG(enum reg r);
    void INC_MEM(u16 addr);
    void DEC_REG(enum reg r);
    void DEC_MEM(u16 addr);
    void AND(u8 data);
    void CMP(u8 data, enum reg r);
    void ADD(u8 data);

    /* Utility functions */
    void PushStack(u8 data);
    void PushStackWord(u16 data);
    void PushStackFlags();
    void TransferReg(u8 *src, u8 *dest);
    u8 PopStack();
    u16 PopStackWord();
    void PopStackFlags();

  public:
    cpu6502();
    void Init();
    void Clock();
    u8 Fetch();
    u16 FetchWord();
    u8 Read(u16 addr);
    void Execute(u8 opcode);
    void StoreByte(u8 val, u16 addr);
};

#endif
