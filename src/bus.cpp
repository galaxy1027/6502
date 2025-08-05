#include "bus.hpp"
#include <iomanip> // for setw, setfill, hex, etc.
#include <iostream>

Bus::Bus()
{
    cpu = new cpu6502();
    ppu = new ppu2C02();

    cpu->ConnectBus(this);

    renderFrame = true;
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
        SDL_PollEvent(&event);
        HandleEvent(event);

        Clock();
        if (renderFrame)
        {
            auto combinedPatternTables = ppu->GetCombinedPatternTables(palette);
            gameRenderer->RenderCombinedTable(combinedPatternTables);
        }
    }
}

void Bus::Startup()
{
    cpu->Init();
    running = true;
    Run();
}

void Bus::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_QUIT)
    {
        running = false;
    }
    else if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_LEFT:
            if (palette > 0)
                palette -= 1;
            break;
        case SDLK_RIGHT:
            if (palette < 4)
                palette += 1;
            break;
        }
    }
}

void Bus::Clock()
{
    ppu->Clock();

    if (systemCycleCount % 3 == 0) // CPU Clock should be 3x slower than PPU
        cpu->Clock();

    if (ppu->nmi)
    {
        ppu->nmi = false;
        // cpu->nmi();
    }

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

void Bus::ConnectToScreen(Renderer *renderer)
{
    gameRenderer = renderer;
}
