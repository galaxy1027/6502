#ifndef MAPPER_H
#define MAPPER_H

#include "types.h"

class Mapper
{
  public:
    Mapper(u8 prgBanks, u8 chrBanks);
    virtual ~Mapper();

  public:
    virtual bool cpuMapRead(u16 addr, u32 &mappedAddr) = 0;
    virtual bool cpuMapWrite(u16 addr, u32 &mappedAddr) = 0;
    virtual bool ppuMapRead(u16 addr, u32 &mappedAddr) = 0;
    virtual bool ppuMapWrite(u16 addr, u32 &mappedAddr) = 0;

  protected:
    u8 prgBankCount;
    u8 chrBankCount;
};

#endif