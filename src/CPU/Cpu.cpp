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

void Cpu::tickTimers() {
    if (registers.delay > 0)
        registers.delay--;
    if (registers.soundTimers > 0)
        registers.soundTimers--;

    vBlank = true;
}

void Cpu::updateKey(const int key, const bool state) {
    if (key >= sizeof (keys) / sizeof (*keys)) {
        std::cerr << "Key out of bounds: " << key << std::endl;
        return;
    }

    keys[key] = state;
    if (waitingForKey && state && waitKey < 0)
        waitKey = key;
    std::cerr << "key " << key << (state ? " down" : " up") << '\n';
}

bool Cpu::isBeeping() {
    return registers.soundTimers > 0;
}

void Cpu::sys() {
    std::cout << "Sys." << std::endl;
}

void Cpu::cls() {
    std::fill(std::begin(display), std::end(display), 0);
}

void Cpu::ret() {
    if (registers.SP <= 0)
        throw std::runtime_error("Stack underflow");

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
    if (!vBlank) {
        registers.PC -= 2;
        return;
    }
    vBlank = false;

    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    const uint8_t n = currentOpcode & 0xf;

    const uint8_t xPos = registers.V[x] % SCREEN_WIDTH;
    const uint8_t yPos = registers.V[y] % SCREEN_HEIGHT;

    registers.V[0xF] = 0;
    const int rows = std::min<int>(n, SCREEN_HEIGHT - yPos);
    const int cols = std::min<int>(8, SCREEN_WIDTH - xPos);

    for (int i = 0; i < rows; i++) {
        const uint8_t sprite = memory[registers.I + i];
        for (int j = 0; j < cols; j++) {
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
    const uint8_t x = currentOpcode >> 8 & 0xf;
    if (x > 0xf)
        throw std::runtime_error("Key doesnt exist");

    if (keys[registers.V[x] &0xf])
        registers.PC += 2;
}

void Cpu::sknp() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    if (x > 0xf)
        throw std::runtime_error("Key doesnt exist");

    if (!keys[registers.V[x] &0xf])
        registers.PC += 2;
}

void Cpu::ldVDt() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.V[x] = registers.delay;
}

void Cpu::ldK() {
    const uint8_t x = currentOpcode >> 8 & 0xf;

    if (!waitingForKey) {
        waitingForKey = true;
        waitKey = -1;
    }

    if (waitKey >= 0 && !keys[waitKey]) {
        registers.V[x] = waitKey;
        waitingForKey = false;
        waitKey = -1;
        return;
    }

    registers.PC -= 2;
}

void Cpu::ldDtV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.delay = registers.V[x];
}

void Cpu::ldStV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.soundTimers = registers.V[x];
}

void Cpu::addIV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.I += registers.V[x];
}

void Cpu::ldFV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    registers.I = 0x50 + 5 * (registers.V[x]&0xf);
}

void Cpu::ldBV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;

    uint8_t number = registers.V[x];
    memory[registers.I + 2] = number % 10;
    number /= 10;
    memory[registers.I + 1] = number % 10;
    number /= 10;
    memory[registers.I] = number % 10;
}

void Cpu::ldIV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    for (int i = 0; i <= x; ++i) {
        memory[registers.I + i] = registers.V[i];
    }
    registers.I += x + 1;
}

void Cpu::ldVI() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    for (int i = 0; i <= x; ++i) {
        registers.V[i] = memory[registers.I + i];
    }
    registers.I += x + 1;
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
    registers.V[0xf] = 0;
}

void Cpu::andVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] &= registers.V[y];
    registers.V[0xf] = 0;
}

void Cpu::xorVV() {
    const uint8_t x = currentOpcode >> 8 & 0xf;
    const uint8_t y = currentOpcode >> 4 & 0xf;
    registers.V[x] ^= registers.V[y];
    registers.V[0xf] = 0;
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
    const uint8_t y = currentOpcode >> 4 & 0xf;
    const uint8_t value = registers.V[y];
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
    const uint8_t y = currentOpcode >> 4 & 0xf;
    const uint8_t value = registers.V[y];

    registers.V[x] = value << 1;
    registers.V[0xF] = (value >> 7) & 1;
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
    if (registers.SP >= 16)
        throw std::runtime_error("Stack Overflow");

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

void Cpu::reset() {
    std::fill(std::begin(memory),std::end(memory),0);
    std::fill(std::begin(display),std::end(display),0);
    std::fill(std::begin(keys),std::end(keys),false);
    std::fill(std::begin(registers.V),std::end(registers.V),0);
    std::fill(std::begin(registers.stack),std::end(registers.stack),0);

    registers.delay=0;
    registers.soundTimers=0;
    waitingForKey=false;
    waitKey=-1;
    vBlank=true;
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
