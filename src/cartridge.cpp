#include "cartridge.hpp"

#include <fstream>
#include <iostream>

Cartridge::Cartridge(std::string path)
{
    std::cout << "ROM PATH: " << path << "\n";
    std::fstream inputFile(path, std::ios::binary | std::ios::in);

    if (inputFile.is_open())
    {
        inputFile.read((char *)&header, sizeof(cartHeader));

        /* Handle trainer being present */
        if (header.flags6 & 0x04)
            inputFile.seekg(512, std::ios_base::cur);

        mapperID = (0x0F & (header.flags6 >> 4)) | (0xF0 & header.flags7);

        u8 inesFileType = 1;

        if (inesFileType == 0)
            ;
        if (inesFileType == 1)
        {
            /* Load program memory */
            prgBankCount = header.prgSizeLSB;
            prgMemory.resize(prgBankCount * 16 * 1024);
            inputFile.read((char *)prgMemory.data(), prgMemory.size());

            /* Load character memory */
            chrBankCount = header.chrSizeMSB;
            prgMemory.resize(chrBankCount * 8 * 1024);
            inputFile.read((char *)chrMemory.data(), chrMemory.size());
        }
        if (inesFileType == 2)
            ;

        /* Load mapper */
        switch (mapperID)
        {
        case 0:
            cartMapper = new Mapper0(prgBankCount, chrBankCount);
            break;
        };
    }
    else
    {
        std::cerr << "Error rom doesn't exist\n";
        exit(1);
    }
    std::cout << "Rom loaded\n";
}

Cartridge::~Cartridge()
{
}

bool Cartridge::cpuRead(u16 addr, u8 &data)
{
    u32 mappedAddr = 0;
    bool status = false;

    if (cartMapper->cpuMapRead(addr, mappedAddr))
    {
        data = prgMemory[mappedAddr];
        status = true;
    }
    return status;
}
bool Cartridge::cpuWrite(u8 data, u16 addr)
{
    u32 mappedAddr = 0;
    bool status = false;

    if (cartMapper->cpuMapRead(addr, mappedAddr))
    {
        prgMemory[mappedAddr] = data;
        status = true;
    }
    return status;
}

bool Cartridge::ppuRead(u16 addr, u8 &data)
{
    u32 mappedAddr = 0;
    bool status = false;

    if (cartMapper->cpuMapRead(addr, mappedAddr))
    {
        data = chrMemory[mappedAddr];
        status = true;
    }
    return status;
}
bool Cartridge::ppuWrite(u16 addr, u8 data)
{
    u32 mappedAddr = 0;
    bool status = false;

    if (cartMapper->cpuMapRead(addr, mappedAddr))
    {
        chrMemory[mappedAddr] = data;
        status = true;
    }
    return status;
}