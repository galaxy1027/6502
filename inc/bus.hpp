#ifndef BUS_HPP
#define BUS_HPP

#include <array>
#include <cstddef>
#include <string>

#include "cartridge.hpp"
#include "cpu6502.hpp"
#include "ppu2C02.hpp"

enum class consoleTypeID
{
    NES = 0,
    NintendoVs = 1,
    NintendoPlaychoice = 2,
    ExtendedConsole = 3
};

class Bus
{
  private:
    /* Hardware on bus*/
    cpu6502 *cpu;
    ppu2C02 *ppu;
    Cartridge *cart;
    std::array<u8, MAX_MEM> ram; // OLD version of ram, delete later
    std::array<u8, 2048> cpuRam;

    /* Header info */
    std::size_t prgRomSize = 0;
    std::size_t chrRomSize = 0;

    /* Flags in byte 6 */
    bool nametableLayout = false;
    bool hasBattery = false;
    bool hasTrainer = false;
    bool alternateNametables = false;
    /* Flags in byte 7 */
    enum consoleTypeID consoleType = consoleTypeID::NES;
    u8 nesIdentifier = 0;
    u8 mapperNumber = 0;

    /* System info */
    u32 systemCycleCount = 0;
    bool running;

    void Clock();

  public:
    Bus();
    ~Bus();
    void Run();
    void Startup();
    int LoadRom(std::string path);
    void parseHeader(std::array<u8, 16> header);

    u8 cpuRead(u16 addr);
    void cpuWrite(u8 data, u16 addr);

    void InsertCartridge(Cartridge *cart);
};

#endif