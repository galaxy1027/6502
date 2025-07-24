#include <array>
#include <fstream>
#include <iostream>

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

        for (int i = 0; i < 16; i++)
        {
            inputFile.read(&byteRead, 1);
            header[i] = byteRead;
        }
    }
    else
    {
        std::cerr << "Error: ROM file does not exist!\n";
        status = -1;
    }

    inputFile.close();

    return status;
}
