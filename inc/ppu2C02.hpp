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

    u32 getColorFromPalette(u8 palette, u8 pixel);

    void UpdatePatternTableVisualize(u8 tableNum, u8 paletteNum);

    SDL_Surface *GetCombinedPatternTables(int palette);

    std::array<u32, 0x40> nesColorPalette = {
        0xFF757575, 0xFF271B8F, 0xFF0000AB, 0xFF47009F, 0xFF8F0077, 0xFFA7003B, 0xFFA70000, 0xFF7F0B00,
        0xFF432F00, 0xFF004700, 0xFF005100, 0xFF003F17, 0xFF1B3F5F, 0xFF000000, 0xFF000000, 0xFF000000,

        0xFFBCBCBC, 0xFF0073EF, 0xFF233BEF, 0xFF8300F3, 0xFFBF00BF, 0xFFE7005B, 0xFFDB2B00, 0xFFCB4F0F,
        0xFF8B7300, 0xFF009700, 0xFF00AB00, 0xFF00933B, 0xFF00838B, 0xFF000000, 0xFF000000, 0xFF000000,

        0xFFFFFFFF, 0xFF3FBFFF, 0xFF5F73FF, 0xFFA78BFD, 0xFFF77BFF, 0xFFFF77B7, 0xFFFF7763, 0xFFFF9B3B,
        0xFFF3BF3F, 0xFF83D313, 0xFF4FDF4B, 0xFF58F898, 0xFF00EBDB, 0xFF000000, 0xFF000000, 0xFF000000,

        0xFFFFFFFF, 0xFFABE7FF, 0xFFC7D7FF, 0xFFD7CBFF, 0xFFFFC7FF, 0xFFFFC7DB, 0xFFFFBFB3, 0xFFFFDBAB,
        0xFFFFE7A4, 0xFFE3FFA3, 0xFFABF3BF, 0xFFB3FFCF, 0xFF9FFFF3, 0xFF000000, 0xFF000000, 0xFF000000};

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