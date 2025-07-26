#ifndef MAPPER_0_HPP
#define MAPPER_0_HPP

#include "mapper.hpp"

class Mapper0 : public Mapper
{
  public:
    Mapper0(u8 prgBanks, u8 chrBanks);
    ~Mapper0();

    virtual bool cpuMapRead(u16 addr, u32 &mappedAddr) override;
    virtual bool cpuMapWrite(u16 addr, u32 &mappedAddr) override;
    virtual bool ppuMapRead(u16 addr, u32 &mappedAddr) override;
    virtual bool ppuMapWrite(u16 addr, u32 &mappedAddr) override;
};

#endif