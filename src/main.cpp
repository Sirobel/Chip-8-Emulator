//
// Created by finns on 24.10.26.
//

#include <fstream>
#include <iostream>
#include <ostream>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include "CPU/Cpu.h"
#include "Screen/Renderer.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Chip-8 Emulator", 640, 320,SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    Cpu cpu;
    Renderer display(renderer);
    std::cout << "Hello World ! " << sizeof (cpu.registers)<<" "<< 36-12 << std::endl;
    constexpr auto path = "/home/finns/CLionProjects/Chip-8-Emulator/roms/5-quirks.ch8";
    std::ifstream file(path);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (size >= 4096 - 0x200)
        std::cout << "size is too big" << std::endl;

    file.read(reinterpret_cast<char *>(&cpu.memory[0x200]), size);

    bool running = true;
    SDL_Event event;

    for (auto i = 0; i < 50000; i++) {
        try {
            cpu.cycle();
            display.renderDisplay(cpu);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            running = false;
            break;
        }

    }


    while (running&& SDL_WaitEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
