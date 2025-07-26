#ifndef PPU_HPP
#define PPU_HPP

#include <array>
#include <cstdint>

#include "cartridge.hpp"
#include "types.h"

#define MAX_MEM (64 * 1024)

class ppu2C02
{
  private:
    /* Register addresses */
  private:
    /* Memory map */
    std::array<std::array<u8, 1024>, 2> nametables; // NES has 2x 1KB tables
    std::array<u8, 32> palettes;

  public:
    Cartridge *cart;

  public:
    ppu2C02();
    ~ppu2C02();
    void Init();
    void Clock();

    /* Main CPU bus*/
    u8 cpuRead(u16 addr);
    void cpuWrite(u8 data, u16 addr);

    /* PPU bus */
    u8 ppuRead(u16 addr);
    void ppuWrite(u16 addr, u8 data);
};

#endif