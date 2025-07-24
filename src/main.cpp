#include <iostream>
#include <string>

#include "nes.hpp"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cerr << "Provide a path to a valid ROM file.\n";
        return 1;
    }

    /* Initialize NES object */
    nes *nesEmu = new nes();
    std::cout << "Initialized...\n";

    /* Load rom */
    std::string romPath = std::string(argv[1]);

    /* Run emulator */
    nesEmu->Startup(romPath);

    nesEmu->Run();

    /* Cleanup */
    delete nesEmu;

    return 0;
}
