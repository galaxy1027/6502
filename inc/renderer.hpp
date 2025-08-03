#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>

#include <array>
#include <iostream>

#include "types.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

class Renderer
{
  public:
    Renderer(int width, int height);
    ~Renderer();

    void RenderFrame(std::array<u32, 256 * 240> buffer);
    void RenderCombinedTable(SDL_Surface *table);
    void Clear();

  private:
    SDL_Window *window;
    SDL_Renderer *sdlRenderer;
    SDL_Texture *texture;
    SDL_Texture *patternTexture;

    uint8_t selectedPalette = 0x00;

    int width;
    int height;
};
#endif