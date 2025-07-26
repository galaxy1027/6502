#include "mapper0.hpp"

Mapper0::Mapper0(u8 prgBanks, u8 chrBanks) : Mapper(prgBanks, chrBanks)
{
}

Mapper0::~Mapper0()
{
}

bool Mapper0::cpuMapRead(u16 addr, u32 &mappedAddr)
{
    bool status = false;
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mappedAddr = addr & (prgBankCount > 1 ? 0x7FFF : 0x3FFF);
        status = true;
    }
    return status;
}

bool Mapper0::cpuMapWrite(u16 addr, u32 &mappedAddr)
{
    bool status = false;
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mappedAddr = addr & (prgBankCount > 1 ? 0x7FFF : 0x3FFF);
        status = true;
    }
    return status;
}

bool Mapper0::ppuMapRead(u16 addr, u32 &mappedAddr)
{
    bool status = false;
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mappedAddr = addr;
        status = true;
    }
    return status;
}

bool Mapper0::ppuMapWrite(u16 addr, u32 &mappedAddr)
{
    return false; // Only ROM in PPU map
}
