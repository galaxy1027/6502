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
    void Clear();

  private:
    SDL_Window *window;
    SDL_Renderer *sdlRenderer;
    SDL_Texture *texture;

    int width;
    int height;
};
#endif