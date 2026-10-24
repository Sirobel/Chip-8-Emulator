//
// Created by finns on 24.10.26.
//

#include "Renderer.h"

Renderer::Renderer(SDL_Renderer *render) {
    renderer = render;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    if (!texture) {
        SDL_Log("Failed to create SDL texture: %s", SDL_GetError());
        return;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void Renderer::renderDisplay(const Cpu &cpu) {
    static uint8_t pixels[64 * 32 * 4];

    for (int i = 0; i < 64 * 32; ++i) {
        const uint8_t color = cpu.display[i] ? 255 : 0;

        pixels[i * 4] = color;
        pixels[i * 4 + 1] = color;
        pixels[i * 4 + 2] = color;
        pixels[i * 4 + 3] = color;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, 64 * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);
}
