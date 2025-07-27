#include "ppu2C02.hpp"

ppu2C02::ppu2C02()
{
    patternTable[0] = SDL_CreateRGBSurfaceWithFormat(0, 128, 128, 32, SDL_PIXELFORMAT_RGBA8888);
}

ppu2C02::~ppu2C02()
{
}

void ppu2C02::Clock()
{
}
/* CPU can only access 8 locations in the PPU, the registers */
u8 ppu2C02::cpuRead(u16 addr)
{
    u8 data = 0x00;

    switch (addr)
    {
    case 0x0002: // Status register
        data = 0x80;
        break;
    }

    return data;
}

/* CPU can only access 8 locations in the PPU, the registers */
void ppu2C02::cpuWrite(u8 data, u16 addr)
{

    switch (addr)
    {
    case 0x2000:
        break;
    case 0x2001:
        break;
    case 0x2002:
        break;
    case 0x2003:
        break;
    case 0x2004:
        break;
    case 0x2005:
        break;
    case 0x2006:
        break;
    case 0x2007:
        break;
    }
}

u8 ppu2C02::ppuRead(u16 addr)
{
    u8 data = 0;
    addr &= 0x3FFF;

    if (cart->ppuRead(addr, data))
        ;

    return data;
}

void ppu2C02::ppuWrite(u16 addr, u8 data)
{
    addr &= 0x3FFF;

    if (cart->cpuWrite(data, addr))
        ;
}

std::array<u32, 128 * 128> ppu2C02::GetPatternTable(u8 tableNum)
{
    std::array<u32, 128 * 128> pixels;

    /* Loop through each tile (256 tiles per table)*/
    for (int tileIndex = 0; tileIndex < 256; tileIndex++)
    {
        int tileX = tileIndex % 16; // the row increases every 16 tiles
        int tileY = tileIndex / 16; // the column loops through 1-16 every index
        u16 tableBaseAddr = 0x1000 * tableNum;
        u16 displacement = tileIndex * 16;

        for (int row = 0; row < 8; row++)
        {
            u8 tileLSB = ppuRead(tableBaseAddr + displacement + row);
            u8 tileMSB = ppuRead(tableBaseAddr + displacement + row + 8);

            for (int col = 0; col < 8; col++)
            {
                u8 pixel = (tileLSB & 0x01) + (tileMSB & 0x01);
                tileLSB >>= 1;
                tileMSB >>= 1;

                /* Grayscale palette */
                u32 color = 0xFF000000;
                switch (pixel)
                {
                case 0:
                    color = 0xFF000000; // Black
                    break;
                case 1:
                    color = 0xFF555555; // Dark grey
                    break;
                case 2:
                    color = 0xFFAAAAAA; // Light grey
                    break;
                case 3:
                    color = 0xFFFFFFFF; // White
                    break;
                }

                int x = tileX * 8 + (7 - col);
                int y = tileY * 8 + row;
                pixels[y * 128 + x] = color;
            }
        }
    }
    return pixels;
}
