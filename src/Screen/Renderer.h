//
// Created by finns on 24.10.26.
//

#ifndef CHIP_8_EMULATOR_RENDERER_H
#define CHIP_8_EMULATOR_RENDERER_H
#include <SDL3/SDL.h>

#include "../CPU/Cpu.h"

class Renderer {
public:
    Renderer(SDL_Renderer *render);
    void renderDisplay(const Cpu &cpu);

private:
    SDL_Texture *texture;
    SDL_Renderer *renderer;
};


#endif //CHIP_8_EMULATOR_RENDERER_H
