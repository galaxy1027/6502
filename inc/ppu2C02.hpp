#ifndef PPU_HPP
#define PPU_HPP

#include <SDL2/SDL.h>

#include <array>
#include <cstdint>
#include <iostream>

#include "cartridge.hpp"
#include "types.h"

#define MAX_MEM (64 * 1024)

/* Definitions for register bits */

#define CTRL_NAMETABLE_X 0x01
#define CTRL_NAMETABLE_Y 0x02
#define CTRL_VRAM_INCREMENT 0x04
#define CTRL_SPRITE_PATTERN_ADDR 0x08
#define CTRL_BG_PATTERN_ADDR 0x10
#define CTRL_SPRITE_SIZE 0x20
#define CTRL_MASTER_SLAVE 0x40 // Unused
#define CTRL_ENABLE_NMI 0x80

// Mask
#define MASK_GREYSCALE 0x01
#define MASK_SHOW_BG_LEFT 0x02
#define MASK_SHOW_SPRITES_LEFT 0x04
#define MASK_SHOW_BACKGROUND 0x08
#define MASK_SHOW_SPRITES 0x10
#define MASK_EMPHASIZE_RED 0x20
#define MASK_EMPHASIZE_GREEN 0x40
#define MASK_EMPHASIZE_BLUE 0x80

// Status
#define STATUS_VERTICAL_BLANK 0x80
#define STATUS_SPRITE_HIT 0x40
#define STATUS_SPRITE_OVERFLOW 0x20

/* Flags in each PPU register */

class ppu2C02
{
  private:
    /* Memory map */

    std::array<std::array<u8, 1024>, 2> nametable; // NES has 2x 1KB tables
    std::array<u8, 32> paletteTable;
    std::array<std::array<u8, 4096>, 2> patternTable;   // Used in tutorial, this memory normally exists on the cart
    std::array<SDL_Surface *, 2> patternTableVisualize; // Debug to view pattern table

  public:
    Cartridge *cart;
    bool nmi;

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

    /* Get a pattern table from CHR memory as an array of pixels */
    std::array<u32, 128 * 128> GetPatternTable(u8 patternTable, u8 paletteNum);

    u32 GetColorFromPalette(u8 palette, u8 pixel);

    void UpdatePatternTableVisualize(u8 tableNum, u8 paletteNum);

    SDL_Surface *GetCombinedPatternTables(int palette);

    std::array<u32, 0x40> nesColorPalette = {
        0x757575FF, 0x271B8FFF, 0x0000ABFF, 0x47009FFF, 0x8F0077FF, 0xA7003BFF, 0xA70000FF, 0x7F0B00FF,
        0x432F00FF, 0x004700FF, 0x005100FF, 0x003F17FF, 0x1B3F5FFF, 0x000000FF, 0x000000FF, 0x000000FF,

        0xBCBCBCFF, 0x0073EFFF, 0x233BEFFF, 0x8300F3FF, 0xBF00BFFF, 0xE7005BFF, 0xDB2B00FF, 0xCB4F0FFF,
        0x8B7300FF, 0x009700FF, 0x00AB00FF, 0x00933BFF, 0x00838BFF, 0x000000FF, 0x000000FF, 0x000000FF,

        0xFFFFFFFF, 0x3FBFFFFF, 0x5F73FFFF, 0xA78BFDFF, 0xF77BFFFF, 0xFF77B7FF, 0xFF7763FF, 0xFF9B3BFF,
        0xF3BF3FFF, 0x83D313FF, 0x4FDF4BFF, 0x58F898FF, 0x00EBDBFF, 0x000000FF, 0x000000FF, 0x000000FF,

        0xFFFFFFFF, 0xABE7FFFF, 0xC7D7FFFF, 0xD7CBFFFF, 0xFFC7FFFF, 0xFFC7DBFF, 0xFFBFB3FF, 0xFFDBABFF,
        0xFFE7A4FF, 0xE3FFA3FF, 0xABF3BFFF, 0xB3FFCFFF, 0x9FFFF3FF, 0x000000FF, 0x000000FF, 0x000000FF};

  private:
    /* PPU Info */
    i16 scanline;
    i16 cycle;

    /* PPU Registers*/

    u8 ppuCtrl;
    u8 ppuMask;
    u8 ppuStatus;

    u8 addressLatch;
    u8 dataBuffer;
    u8 ppuAddress;
};
#endif