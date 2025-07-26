#ifndef BUS_HPP
#define BUS_HPP

#include <SDL2/SDL.h>

#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cartridge.hpp"
#include "cpu6502.hpp"
#include "ppu2C02.hpp"
#include "renderer.hpp"

class Bus
{
  private:
    /* Hardware on bus*/
    cpu6502 *cpu;
    ppu2C02 *ppu;
    Cartridge *cart;
    std::array<u8, 2048> cpuRam;

    /* Header info */
    std::size_t prgRomSize = 0;
    std::size_t chrRomSize = 0;

    /* System info */
    u32 systemCycleCount = 0;
    bool renderFrame; // Used for the renderer to update the screen with a new frame

    /* Graphics */
    Renderer *gameRenderer;
    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> framebuffer;
    SDL_Event sdlEvent;

    void Clock();

  public:
    Bus();
    ~Bus();
    void Run();
    void Startup();

    u8 cpuRead(u16 addr);
    void cpuWrite(u8 data, u16 addr);

    void InsertCartridge(Cartridge *cart);
    void ConnectToScreen(Renderer *renderer);
    bool running;
};
#endif