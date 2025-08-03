#include "ppu2C02.hpp"

ppu2C02::ppu2C02()
{
    patternTableVisualize[0] = SDL_CreateRGBSurfaceWithFormat(0, 128, 128, 32, SDL_PIXELFORMAT_RGBA8888);
    patternTableVisualize[1] = SDL_CreateRGBSurfaceWithFormat(0, 128, 128, 32, SDL_PIXELFORMAT_RGBA8888);

    nesColorPalette = {0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800,
                       0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00, 0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
                       0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
                       0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
                       0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC, 0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820,
                       0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
                       0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAAE4, 0xFFECB4C4, 0xFFE4C0A0, 0xFFE0D084,
                       0xFFCCE482, 0xFFAEEA8C, 0xFFB4F4BC, 0xFFB0F0D8, 0xFFB8E8E4, 0xFFB8B8B8, 0xFF000000, 0xFF000000};
    paletteTable.fill(0x00);
    // Set background palette #0 to black, dark gray, light gray, white
    paletteTable[0x00] = 0x00; // color index 0 (still gray)
    paletteTable[0x01] = 0x01; // color index 1 = dark blue
    paletteTable[0x02] = 0x02; // color index 2 = light blue
    paletteTable[0x03] = 0x03; // color index 3 = magenta

    // Optional: background palette #1
    paletteTable[0x04] = 0x10; // color index from later in palette
    paletteTable[0x05] = 0x20;
    paletteTable[0x06] = 0x30;
    paletteTable[0x07] = 0x3F;
}

ppu2C02::~ppu2C02()
{
}

void ppu2C02::Clock()
{

    if (scanline == -1 && cycle == 1)
    {
        ppuStatus &= ~STATUS_VERTICAL_BLANK;
    }

    if (scanline == 241 && cycle == 1)
    {
        ppuStatus |= STATUS_VERTICAL_BLANK;
        if (ppuCtrl & CTRL_ENABLE_NMI)
        {
            nmi = true;
        }
    }
}
/* CPU can only access 8 locations in the PPU, the registers */
u8 ppu2C02::cpuRead(u16 addr)
{
    u8 data = 0x00;
    switch (addr)
    {
    case 0x2000: // Control
        break;
    case 0x2001: // Mask
        break;
    case 0x2002: // Status
        data = (ppuStatus & 0xE0);
        ppuStatus &= ~STATUS_VERTICAL_BLANK; // Clear vertical blank
        addressLatch = 0;
        break;
    case 0x2003:
        break;
    case 0x2004:
        break;
    case 0x2005:
        break;
    case 0x2006: // Address, can't read

        break;
    case 0x2007: // PPU Data
        data = dataBuffer;
        dataBuffer = ppuRead(ppuAddress);

        if (ppuAddress > 0x3F00) // Palette data is read instantaneously
            data = dataBuffer;
        ppuAddress++;
        break;
    }
    return data;
}

/* CPU can only access 8 locations in the PPU, the registers */
void ppu2C02::cpuWrite(u8 data, u16 addr)
{

    switch (addr)
    {
    case 0x2000: // Control
        ppuCtrl = data;
        break;
    case 0x2001: // Mask
        ppuMask = data;
        break;
    case 0x2002: // Status
        break;
    case 0x2003:
        break;
    case 0x2004:
        break;
    case 0x2005:
        break;
    case 0x2006: // Address
        if (addressLatch == 0)
        {

            ppuAddress = (ppuAddress & 0x00FF) | (data << 8);
            addressLatch = 1;
        }
        else
        {
            ppuAddress = (ppuAddress & 0xFF00) | data; // Set lower 8 bits
            addressLatch = 0;
        }
        break;
    case 0x2007: // PPU Data
        ppuWrite(ppuAddress, data);
        ppuAddress++;
        break;
    }
}

u8 ppu2C02::ppuRead(u16 addr)
{
    u8 data = 0;
    addr &= 0x3FFF;

    if (cart->ppuRead(addr, data))
        ;
    else if (addr >= 0x0000 && addr <= 0x1FFF) // Reading from pattern memory
    {
        data = patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) // Reading from nametable
    {
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) // Reading from system color palette
    {
        addr &= 0x001F;

        /* Hard coded in mirrored addresses */
        if (addr == 0x0010)
            addr = 0x0000;
        if (addr == 0x0014)
            addr = 0x0004;
        if (addr == 0x0018)
            addr = 0x0008;
        if (addr == 0x001C)
            addr = 0x000C;

        data = paletteTable[addr];
    }

    return data;
}

void ppu2C02::ppuWrite(u16 addr, u8 data)
{
    addr &= 0x3FFF;

    if (cart->cpuWrite(data, addr))
        ;
    else if (addr >= 0x0000 && addr <= 0x1FFF) // writing pattern memory
    {
        patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) // writing nametable
    {
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) // writing system color palette
    {
        addr &= 0x001F;

        /* Hard coded in mirrored addresses */
        if (addr == 0x0010)
            addr = 0x0000;
        if (addr == 0x0014)
            addr = 0x0004;
        if (addr == 0x0018)
            addr = 0x0008;
        if (addr == 0x001C)
            addr = 0x000C;

        paletteTable[addr] = data;
    }
}
std::array<u32, 128 * 128> ppu2C02::GetPatternTable(u8 tableNum, u8 paletteNum)
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

                u32 color = getColorFromPalette(paletteNum, pixel);
                int x = tileX * 8 + (7 - col);
                int y = tileY * 8 + row;
                pixels[y * 128 + x] = color;
            }
        }
    }
    return pixels;
}

u32 ppu2C02::getColorFromPalette(u8 palette, u8 pixel)
{
    return nesColorPalette[ppuRead(0x3F00 + (palette * 4) + pixel) & 0x3F];
}

/* Debug */
void ppu2C02::UpdatePatternTableVisualize(u8 tableNum, u8 paletteNum)
{
    // Get pixels from your existing function
    std::array<u32, 128 * 128> pixels = GetPatternTable(tableNum, paletteNum);

    SDL_Surface *surf = patternTableVisualize[tableNum];
    if (!surf)
        return;

    SDL_LockSurface(surf);
    // Copy pixels into surface pixel buffer
    // Assuming surf->format is ARGB8888 or similar 32-bit format
    u32 *surfPixels = (u32 *)surf->pixels;
    std::copy(pixels.begin(), pixels.end(), surfPixels);
    SDL_UnlockSurface(surf);
}

SDL_Surface *ppu2C02::GetCombinedPatternTables(int palette)
{
    auto leftPixels = GetPatternTable(0, palette);
    SDL_Surface *leftSurface = SDL_CreateRGBSurfaceFrom(leftPixels.data(), 128, 128, 32, 128 * sizeof(u32), 0x000000ff,
                                                        0x0000ff00, 0x00ff0000, 0xff000000);
    auto rightPixels = GetPatternTable(1, palette);
    SDL_Surface *rightSurface = SDL_CreateRGBSurfaceFrom(rightPixels.data(), 128, 128, 32, 128 * sizeof(u32),
                                                         0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    // Create a surface 256x128 to hold both side-by-side
    SDL_Surface *combined = SDL_CreateRGBSurfaceWithFormat(0, 256, 128, 32, SDL_PIXELFORMAT_RGBA8888);

    // Blit leftSurface at (0, 0)
    SDL_Rect leftRect = {0, 0, 128, 128};
    SDL_BlitSurface(leftSurface, nullptr, combined, &leftRect);

    // Blit rightSurface at (128, 0)
    SDL_Rect rightRect = {128, 0, 128, 128};
    SDL_BlitSurface(rightSurface, nullptr, combined, &rightRect);

    // SDL_CreateRGBSurfaceFrom does NOT own pixel data, so just free the surfaces (no pixel free)
    SDL_FreeSurface(leftSurface);
    SDL_FreeSurface(rightSurface);
    return combined;
}
