#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "cpu6502.hpp"
#include "nes.hpp"

nes::nes()
{
    cpu = new cpu6502();
}

nes::~nes()
{
    delete cpu;
}

void nes::Run()
{
    while (1)
    {
        cpu->Clock();
    }
}

void nes::Startup(std::string romPath)
{
    if (LoadRom(romPath) == 0)
    {
        std::cout << "Program loaded!\n";
        cpu->Init();
    }
    else
    {
        std::cerr << "Failed to load rom, aborting";
        exit(1);
    }
}

/*
 * Load NES Rom
 *
 * If the file does not exist, -1 is returned for error handling
 */
int nes::LoadRom(std::string path)
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
        std::copy(prgRom.begin(), prgRom.end(), cpu->ram.begin() + 0x8000);

        if (prgRomSize == 16 * 1024)
            // Mirror 16K rom into 32K space
            std::copy(prgRom.begin(), prgRom.end(), cpu->ram.begin() + 0xC000);
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
void nes::parseHeader(std::array<u8, 16> header)
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
