#include <iostream>
#include <string>

#include "bus.hpp"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cerr << "Provide a path to a valid ROM file.\n";
        return 1;
    }

    /* Initialize NES object */
    Bus *nesEmu = new Bus();
    std::cout << "Initialized...\n";

    /* Load rom */
    Cartridge *cart = new Cartridge(std::string(argv[1]));
    nesEmu->InsertCartridge(cart);

    /* Run emulator */
    nesEmu->Startup();

    /* Cleanup */
    delete nesEmu;

    return 0;
}
