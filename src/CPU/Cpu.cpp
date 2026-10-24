//
// Created by finns on 24.10.26.
//

#include "Cpu.h"

#include <iostream>
#include <ostream>
#include <random>

#include "CpuInstructions.h"

void Cpu::cycle() {
    fetchOpcode();
    executeOpcode();
}

void Cpu::sys() {
}

void Cpu::cls() {
    std::fill(std::begin(display), std::end(display), 0);
}

void Cpu::ret() {
    registers.SP--;
    registers.PC = registers.stack[registers.SP];
}

void Cpu::ldVB() {
    const uint8_t x = currentOpcode >> 8 & 0x0f;
    const uint8_t kk = currentOpcode & 0xff;

    registers.V[x] = kk;
}

void Cpu::LdI() {
    const uint16_t nnn = currentOpcode & 0xfff;
    registers.I = nnn;
}

void Cpu::jpV() {
    const uint16_t nnn = currentOpcode & 0xfff;
    registers.PC = nnn + registers.V[0];
}

void Cpu::rnd() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t kk = currentOpcode & 0xff;

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, 255);

    const uint8_t random = distribution(generator);
    registers.V[x] = kk & random;
}

void Cpu::drw() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    const uint8_t n = currentOpcode & 0xf;

    const uint8_t xPos = registers.V[x] % SCREEN_WIDTH;
    const uint8_t yPos = registers.V[y] % SCREEN_HEIGHT;

    registers.V[0xF] = 0;

    for (uint8_t i = 0; i < n; i++) {
        const uint8_t sprite = memory[registers.I + i];
        for (uint8_t j = 0; j < 8; j++) {
            if ((sprite & (0x80 >> j)) == 0)
                continue;

            const int pixelX = (xPos + j) % SCREEN_WIDTH;
            const int pixelY = (yPos + i) % SCREEN_HEIGHT;

            const int index = pixelY * SCREEN_WIDTH + pixelX;
            if (display[index] == 1)
                registers.V[0xF] = 1;
            display[index] ^= 1;
        }
    }
}

void Cpu::skp() {
}

void Cpu::addIV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.I += registers.V[x];
}

void Cpu::ldBV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;

    uint8_t number = registers.V[x];
    memory[registers.I +2] = number%10;
    number /=10;
    memory[registers.I +1] = number%10;
    number /=10;
    memory[registers.I] = number%10;
}

void Cpu::ldIV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    for (int i = 0; i <= x; ++i) {
        memory[registers.I + i] = registers.V[i];
    }
}

void Cpu::ldVI() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    for (int i = 0; i <= x; ++i) {
         registers.V[i] = memory[registers.I + i];
    }
}

void Cpu::add() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t kk = currentOpcode & 0xff;

    registers.V[x] += kk;
}

void Cpu::ldVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] = registers.V[y];
}

void Cpu::orVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] |= registers.V[y];
}

void Cpu::andVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] &= registers.V[y];
}

void Cpu::xorVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] ^= registers.V[y];
}

void Cpu::addVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;

    const uint16_t res = registers.V[x] + registers.V[y];
    registers.V[x] = res & 0xFF;
    if (res > 0xff)
        registers.V[0xF] = 1;
    else
        registers.V[0xF] = 0;
}

void Cpu::subVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;

    const uint8_t vx = registers.V[x];
    const uint8_t vy = registers.V[y];

    registers.V[x] = vx - vy;
    registers.V[0xF] = (vx >= vy) ? 1 : 0;
}

void Cpu::shrV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t value = registers.V[x];
    registers.V[x] = value >> 1;
    registers.V[0xF] = value & 1;
}

void Cpu::subnVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;

    const uint8_t vx = registers.V[x];
    const uint8_t vy = registers.V[y];

    registers.V[x] = vy - vx;
    registers.V[0xF] = (vx <= vy) ? 1 : 0;

}

void Cpu::shlV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t value = registers.V[x];

    registers.V[x] <<= 1;
    registers.V[0xF] = (value>>7) & 1;
}

void Cpu::sneVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    if (registers.V[x] != registers.V[y])
        registers.PC += 2;
}

void Cpu::jp() {
    const uint16_t nnn = currentOpcode & 0xfff;
    registers.PC = nnn;
}

void Cpu::call() {
    const uint16_t nnn = currentOpcode & 0xfff;
    registers.stack[registers.SP] = registers.PC;
    registers.SP++;
    registers.PC = nnn;
}

void Cpu::seVB() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint16_t kk = currentOpcode & 0xff;

    if (registers.V[x] == kk)
        registers.PC += 2;
}

void Cpu::sneVB() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint16_t kk = currentOpcode & 0xff;

    if (registers.V[x] != kk)
        registers.PC += 2;
}

void Cpu::seVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;

    if (registers.V[x] == registers.V[y])
        registers.PC += 2;
}


Cpu::Cpu() {
    initiate();
}

void Cpu::initiateFontSet() {
    for (int i = 0; i < 80; ++i) {
        memory[0x50 + i] = fontSet[i];
    }
}

void Cpu::initiate() {
    registers.PC = 0x200;
    registers.I = 0;
    registers.SP = 0;

    initiateFontSet();
}

void Cpu::fetchOpcode() {
    currentOpcode = memory[registers.PC] << 8 | memory[registers.PC + 1];
    registers.PC += 2;
    currentInstruction = findInstruction(currentOpcode);
}

void Cpu::executeOpcode() {
    if (!currentInstruction) {
        std::cout << "Unknown Instruction " << std::hex << "0x" << currentOpcode << std::endl;
        throw std::runtime_error("Unknown Instruction");
        return;
    }
    (this->*currentInstruction)();
}
