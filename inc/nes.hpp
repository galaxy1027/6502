#include <cstddef>
#include <string>

#include "cpu6502.hpp"

enum class consoleTypeID
{
    NES = 0,
    NintendoVs = 1,
    NintendoPlaychoice = 2,
    ExtendedConsole = 3
};

class nes
{
  private:
    /* Hardware */
    cpu6502 *cpu;

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

  public:
    nes();
    ~nes();
    void Run();
    void Startup(std::string romPath);
    int LoadRom(std::string path);
    void parseHeader(std::array<u8, 16> header);
};
