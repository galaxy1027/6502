#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "bus.hpp"
#include "cpu6502.hpp"
#include "ppu2C02.hpp"

Bus::Bus()
{
    cpu = new cpu6502();
    ppu = new ppu2C02();
    ram.fill(0);
    cpu->ram = &(this->ram);

    cpu->ConnectBus(this);
}

Bus::~Bus()
{
    delete cpu;
    delete ppu;
    delete cart;
}

void Bus::Run()
{
    while (running)
    {
        Clock();
    }
}

void Bus::Startup()
{
    cpu->Init();
    running = true;
    Run();
}

void Bus::Clock()
{
    ppu->Clock();

    if (systemCycleCount % 3 == 0) // CPU Clock should be 3x slower than PPU
        cpu->Clock();

    systemCycleCount++;
}

u8 Bus::cpuRead(u16 addr)
{
    u8 data = 0x00;

    if (cart->cpuRead(addr, data))
        ;

    else if (addr >= 0x0000 && addr <= 0x1FFF)
        data = cpuRam[addr & 0x07FF];

    else if (addr >= 0x2000 && addr <= 0x3FFF)
        data = ppu->cpuRead(addr & 0x0007);

    return data;
}

void Bus::cpuWrite(u8 data, u16 addr)
{
    if (cart->cpuWrite(data, addr))
    {
    }
    else if (addr > 0x0000 && addr <= 0x1FFF)
        data = cpuRam[addr & 0x07FF];

    else if (addr >= 0x2000 && addr <= 0x3FFF)
        ppu->cpuWrite(addr & 0x0007, data);
}

void Bus::InsertCartridge(Cartridge *cart)
{
    this->cart = cart;
    ppu->cart = cart;
    cpu->cart = cart;
}
/*
 * Load NES Rom
 *
 * If the file does not exist, -1 is returned for error handling
 */
int Bus::LoadRom(std::string path)
{
    int status = 0;

    std::cout << "ROM PATH: " << path << "\n";
    std::fstream inputFile(path, std::ios::binary | std::ios::in);

    std::array<u8, 16> header = {0};

    if (inputFile.is_open())
    {
        char byteRead = 0;

        /* Extract header information */
        for (int i = 0; i < 16; i++)
        {
            inputFile.read(&byteRead, 1);
            header[i] = byteRead;
        }
        parseHeader(header);

        /* Calculate the offset from the beginning of ROM where the PRG_ROM starts */
        size_t romOffsetBytes = 16;
        if (hasTrainer)
            romOffsetBytes += 512;

        /* Extract PRG_ROM and put it into memory */
        std::vector<u8> prgRom(prgRomSize, 0);
        inputFile.seekg(romOffsetBytes);
        inputFile.read(reinterpret_cast<char *>(prgRom.data()), prgRomSize);
        std::copy(prgRom.begin(), prgRom.end(), ram.begin() + 0x8000);

        if (prgRomSize == 16 * 1024)
            // Mirror 16K rom into 32K space
            std::copy(prgRom.begin(), prgRom.end(), ram.begin() + 0xC000);
    }
    else
    {
        std::cerr << "Error: ROM file does not exist!\n";
        status = -1;
    }

    inputFile.close();

    return status;
}

/*
 * Parse Header
 * Extract important information from a headered rom file, like PRG-ROM size
 */
void Bus::parseHeader(std::array<u8, 16> header)
{
    /* Extracting the PRG_ROM size */
    u8 prgRomSizeLSB = header[4];
    u8 prgRomSizeMSB = header[9] & 0x0F;

    if (prgRomSizeMSB == 0xF) // Exponential notation is used
        std::cerr << "Error: exponential PRG_ROM size not yet implemented\n";
    else
        this->prgRomSize = 16 * 1024 * (prgRomSizeLSB | (prgRomSizeMSB << 8));

    if (prgRomSize != 16 * 1024 && prgRomSize != 32 * 1024)
    {
        std::cerr << "Unsupported PRG_ROM size (" << prgRomSize / 1024 << " KB)\n";
        exit(1);
    }

    /* Extracting the CHR_ROM size */
    u8 chrRomSizeLSB = header[5];
    u8 chrRomSizeMSB = (header[9] >> 4) & 0x0F;
    if (prgRomSizeMSB == 0xF)
        std::cerr << "Error: exponential CHR_ROM size not yet implemented\n";
    else
        this->chrRomSize = 8 * 1024 * (chrRomSizeLSB | (chrRomSizeMSB << 8));

    /* Check if a trainer section exists */
    hasTrainer = header[6] & 0x4;
}
