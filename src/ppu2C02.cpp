#include "ppu2C02.hpp"

ppu2C02::ppu2C02()
{
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

    if (cart->cpuRead(addr, data))
        ;

    return data;
}

void ppu2C02::ppuWrite(u16 addr, u8 data)
{
    addr &= 0x3FFF;

    if (cart->cpuWrite(data, addr))
        ;
}