#include "bus.hpp"

Bus::Bus()
{
    cpu = new cpu6502();
    ppu = new ppu2C02();

    cpu->ConnectBus(this);

    renderFrame = true;

    // TODO: Delete frame buffer test
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            uint8_t bar = x / 32;        // 8 vertical bars
            uint32_t color = 0xFF000000; // Alpha

            switch (bar)
            {
            case 0:
                color |= 0xFF0000;
                break; // Red
            case 1:
                color |= 0xFF8000;
                break; // Orange
            case 2:
                color |= 0xFFFF00;
                break; // Yellow
            case 3:
                color |= 0x00FF00;
                break; // Green
            case 4:
                color |= 0x00FFFF;
                break; // Cyan
            case 5:
                color |= 0x0000FF;
                break; // Blue
            case 6:
                color |= 0x8000FF;
                break; // Purple
            case 7:
                color |= 0xFFFFFF;
                break; // White
            }

            framebuffer[y * SCREEN_WIDTH + x] = color;
        }
    }
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
        SDL_PollEvent(&sdlEvent);
        if (sdlEvent.type == SDL_QUIT)
            running = false;
        else
        {
            Clock();
            if (renderFrame)
                gameRenderer->RenderFrame(framebuffer);
        }
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

void Bus::ConnectToScreen(Renderer *renderer)
{
    gameRenderer = renderer;
}
