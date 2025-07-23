#include "cpu6502.hpp"
#include "iostream"

int main(int argc, char **argv)
{
    cpu6502 *cpu = new cpu6502();
    std::cout << "Initialized...\n";
    std::cout << "Program loaded!\n";

    bool running = true;

    while (running)
    {
        cpu->Clock();
    }

    delete cpu;
    return 0;
}
