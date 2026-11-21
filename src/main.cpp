//
// Created by finns on 24.10.26.
//

#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>
#include <vector>
#include <filesystem>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include "CPU/Cpu.h"
#include "Peripherals/Beeper.h"
#include "Peripherals/Renderer.h"

namespace {
    std::string rom;
    std::mutex pendingMutex;

    constexpr int CYCLES_PER_FRAME = 10;
    constexpr uint64_t FRAME_NS = 1'000'000'000 / 60;

    const SDL_DialogFileFilter ROM_FILTERS[] = {
        {"CHIP-8 ROMs","ch8;c8;rom"},
        {"ALL files","*"},
    };

    int mapKey(const SDL_Scancode sc) {
        switch (sc) {
            case SDL_SCANCODE_KP_PERIOD: return 0;
            case SDL_SCANCODE_KP_7: return 1;
            case SDL_SCANCODE_KP_8: return 2;
            case SDL_SCANCODE_KP_9: return 3;
            case SDL_SCANCODE_KP_4: return 4;
            case SDL_SCANCODE_KP_5: return 5;
            case SDL_SCANCODE_KP_6: return 6;
            case SDL_SCANCODE_KP_1: return 7;
            case SDL_SCANCODE_KP_2: return 8;
            case SDL_SCANCODE_KP_3: return 9;
            case SDL_SCANCODE_KP_0: return 0xa;
            case SDL_SCANCODE_KP_PLUS: return 0xb;
            case SDL_SCANCODE_KP_DIVIDE: return 0xc;
            case SDL_SCANCODE_KP_MULTIPLY: return 0xd;
            case SDL_SCANCODE_KP_MINUS: return 0xe;
            case SDL_SCANCODE_KP_ENTER: return 0xf;
            default:
                std::cerr << "pressed " << std::hex << sc << std::endl;
                return -1;
        }
    }

    bool loadRom(Cpu &cpu, const char *romPath) {
        std::ifstream file(romPath, std::ios::binary);
        if (!file) {
            std::cerr << "Error loading rom " << romPath << std::endl;
            return false;
        }
        const std::vector data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (data.size() > 4096 - 0x200) {
            std::cerr << "ROM too big" << std::endl;
            return false;
        }
        std::copy(data.begin(), data.end(), cpu.memory + 0x200);
        return true;
    }

    void SDLCALL onRomSelected(void *, const char *const *files, int) {
        if (!files || !files[0]) return;
        std::lock_guard lock(pendingMutex);
        rom = files[0];
    }

    void openRomDialog(SDL_Window *window) {
        auto currentDir = std::filesystem::current_path();

        SDL_ShowOpenFileDialog(onRomSelected,nullptr,window,ROM_FILTERS,2,currentDir.c_str(),false);
    }
}


int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *window = SDL_CreateWindow("Chip-8 Emulator", 640, 320,SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    Cpu cpu;
    Renderer display(renderer);
    Beeper beeper;
    beeper.init();

    openRomDialog(window);

    bool running = true;
    bool emulating = false;
    uint64_t nextFrame = SDL_GetTicksNS() + FRAME_NS;


    while (running) {
        std::string toLoad;
        {
            std::lock_guard lock(pendingMutex);
            toLoad.swap(rom);
        }
        if (!toLoad.empty()) {
            cpu.reset();
            if (loadRom(cpu,toLoad.c_str())) {
                emulating = true;
            }
        }

        const uint64_t now = SDL_GetTicksNS();
        const int32_t timeoutMS = now < nextFrame ? static_cast<int32_t>((nextFrame - now + 999'999) / 1'000'000) : 0;

        SDL_Event event;
        bool gotEvent = SDL_WaitEventTimeout(&event, timeoutMS);
        while (gotEvent) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                        running = false;
                        break;
                    }
                    if (event.key.repeat)
                        break;
                    if (event.key.scancode == SDL_SCANCODE_E) {
                        emulating = !emulating;
                        break;
                    }
                    const int key = mapKey(event.key.scancode);
                    if (key >= 0)
                        cpu.updateKey(key, event.type == SDL_EVENT_KEY_DOWN);
                    break;
                }
                default:
                    break;
            }

            gotEvent = SDL_PollEvent(&event);
        }

        if (SDL_GetTicksNS() >= nextFrame) {
            if (emulating) {
                try {
                    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                        cpu.cycle();
                    }
                    cpu.tickTimers();
                } catch (const std::exception &e) {
                    std::cerr << e.what() << std::endl;
                    emulating = false;
                }
            }
            display.renderDisplay(cpu);
            beeper.set(emulating && cpu.isBeeping());

            nextFrame = SDL_GetTicksNS() + FRAME_NS;
            const uint64_t after = SDL_GetTicksNS();
            if (nextFrame + FRAME_NS < after) nextFrame = after + FRAME_NS;
        }
    }

    beeper.shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
