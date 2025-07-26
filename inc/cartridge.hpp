#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "mapper0.hpp"
#include "types.h"

#include <array>
#include <string>
#include <vector>

typedef struct cartHeader
{
    std::array<char, 4> name;
    u8 prgSizeLSB;
    u8 chrSizeMSB;
    u8 flags6;
    u8 flags7;
    u8 mapper;
    u8 prgChrMSB;
    u8 prgRamSize;
    u8 chrRamSize;
    std::array<char, 4> unused;
} cartHeader;

class Cartridge
{

  private:
    cartHeader header;
    std::vector<u8> prgMemory;
    std::vector<u8> chrMemory;
    Mapper *cartMapper;

    u8 mapperID = 0;
    u8 prgBankCount = 0;
    u8 chrBankCount = 0;

  public:
    Cartridge(std::string path);
    ~Cartridge();

    /* Main CPU bus*/
    bool cpuRead(u16 addr, u8 &data);
    bool cpuWrite(u8 data, u16 addr);

    /* PPU bus */
    bool ppuRead(u16 addr, u8 &data);
    bool ppuWrite(u16 addr, u8 data);
};

#endif