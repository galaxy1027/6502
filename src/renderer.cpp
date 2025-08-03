#include "renderer.hpp"

Renderer::Renderer(int width, int height)
{

    this->width = width;
    this->height = height;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL Failed to initialize!\n";
        exit(1);
    }

    window = SDL_CreateWindow("NES_EMU",
                              SDL_WINDOWPOS_CENTERED, // Center X pos
                              SDL_WINDOWPOS_CENTERED, // Center Y pos
                              width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    patternTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 128);

    SDL_RenderSetLogicalSize(sdlRenderer, 256, 128); // Match your surface size
}

Renderer::~Renderer()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}

void Renderer::RenderFrame(std::array<u32, 256 * 240> buffer)
{
    SDL_UpdateTexture(texture, nullptr, buffer.data(), width * sizeof(u32));
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::RenderCombinedTable(SDL_Surface *table)
{
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, table);
    SDL_RenderClear(sdlRenderer);
    SDL_Rect destRect = {0, 0, table->w, table->h};
    SDL_RenderCopy(sdlRenderer, texture, nullptr, &destRect);
    SDL_RenderPresent(sdlRenderer);
    SDL_DestroyTexture(texture);
}
void Renderer::Clear()
{
    SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);
}
