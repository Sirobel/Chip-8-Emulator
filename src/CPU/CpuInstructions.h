//
// Created by finns on 24.10.26.
//

#ifndef CHIP_8_EMULATOR_CPUINSTRUCTIONS_H
#define CHIP_8_EMULATOR_CPUINSTRUCTIONS_H
#include <string>

#include "Cpu.h"


struct cpu_instructions {
    uint16_t mask;
    uint16_t patter;
    std::string opcode;

    void (Cpu::*execute)();
};

extern const cpu_instructions instructions[] = {
    {.mask = 0xffff, .patter = 0x00e0, .opcode = "00E0 - CLS", .execute = &Cpu::cls},
    {.mask = 0xffff, .patter = 0x00ee, .opcode = "00EE - RET", .execute = &Cpu::ret},
    {.mask = 0xf000, .patter = 0x1000, .opcode = "1nnn - JP addr", .execute = &Cpu::jp},
    {.mask = 0xf000, .patter = 0x2000, .opcode = "2nnn - CALL addr", .execute = &Cpu::call},
    {.mask = 0xf000, .patter = 0x3000, .opcode = "3xkk - SE Vx, byte", .execute = &Cpu::seVB},
    {.mask = 0xf000, .patter = 0x4000, .opcode = "4xkk - SNE Vx, byte", .execute = &Cpu::sneVB},
    {.mask = 0xf00f, .patter = 0x5000, .opcode = "5xy0 - SE Vx, Vy", .execute = &Cpu::seVV},
    {.mask = 0xf000, .patter = 0x6000, .opcode = "6xkk - LD Vx, byte", .execute = &Cpu::ldVB},
    {.mask = 0xf000, .patter = 0x7000, .opcode = "7xkk - ADD Vx, byte", .execute = &Cpu::add},
    {.mask = 0XF00F, .patter = 0x8000, .opcode = "8xy0 - LD Vx, Vy", .execute = &Cpu::ldVV},
    {.mask = 0xf00f, .patter = 0x8001, .opcode = "8xy1 - OR Vx, Vy", .execute = &Cpu::orVV},
    {.mask = 0xf00f, .patter = 0x8002, .opcode = "8xy2 - AND Vx, Vy", .execute = &Cpu::andVV},
    {.mask = 0xf00f, .patter = 0x8003, .opcode = "8xy3 - XOR Vx, Vy", .execute = &Cpu::xorVV},
    {.mask = 0xf00f, .patter = 0x8004, .opcode = "8xy4 - ADD Vx, Vy", .execute = &Cpu::addVV},
    {.mask = 0xf00f, .patter = 0x8005, .opcode = "8xy5 - SUB Vx, Vy", .execute = &Cpu::subVV},
    {.mask = 0xf00f, .patter = 0x8006, .opcode = "8xy6 - SHR Vx {, Vy}", .execute = &Cpu::shrV},
    {.mask = 0xf00f, .patter = 0x8007, .opcode = "8xy7 - SUBN Vx, Vy", .execute = &Cpu::subnVV},
    {.mask = 0xf00f, .patter = 0x800e, .opcode = "8xyE - SHL Vx {, Vy}", .execute = &Cpu::shlV},
    {.mask = 0xf00f, .patter = 0x9000, .opcode = "9xy0 - SNE Vx, Vy", .execute = &Cpu::sneVV},
    {.mask = 0xf000, .patter = 0xa000, .opcode = "Annn - LD I, addr", .execute = &Cpu::LdI},
    {.mask = 0xf000, .patter = 0xb000, .opcode = "Bnnn - JP V0, addr", .execute = &Cpu::jpV},
    {.mask = 0xf000, .patter = 0xc000, .opcode = "Cxkk - RND Vx, byte", .execute = &Cpu::rnd},
    {.mask = 0xf000, .patter = 0xd000, .opcode = "Dxyn - DRW Vx, Vy, nibble", .execute = &Cpu::drw},
    {.mask = 0xf0ff, .patter = 0xe09e, .opcode = "Ex9E - SKP Vx", .execute = &Cpu::skp},
    {.mask = 0xf0ff, .patter = 0xe0a1, .opcode = "ExA1 - SKNP Vx", .execute = &Cpu::sknp},
    {.mask = 0xf0ff, .patter = 0xf007, .opcode = "Fx07 - LD Vx, DT", .execute = &Cpu::ldVDt},
    {.mask = 0xf0ff, .patter = 0xf00a, .opcode = "Fx0A - LD Vx, K", .execute = &Cpu::ldK},
    {.mask = 0xf0ff, .patter = 0xf015, .opcode = "Fx15 - LD DT, Vx", .execute = &Cpu::ldDtV},
    {.mask = 0xf0ff, .patter = 0xf018, .opcode = "Fx18 - LD ST, Vx", .execute = &Cpu::ldStV},
    {.mask = 0xf0ff, .patter = 0xf01e, .opcode = "Fx1E - ADD I, Vx", .execute = &Cpu::addIV},
    {.mask = 0xf0ff, .patter = 0xf029, .opcode = "Fx29 - LD F, Vx", .execute = &Cpu::ldFV},
    {.mask = 0xf0ff, .patter = 0xf033, .opcode = "Fx33 - LD B, Vx", .execute = &Cpu::ldBV},
    {.mask = 0xf0ff, .patter = 0xf055, .opcode = "Fx55 - LD [I], Vx", .execute = &Cpu::ldIV},
    {.mask = 0xf0ff, .patter = 0xf065, .opcode = "Fx65 - LD Vx, [I]", .execute = &Cpu::ldVI},
    {.mask = 0xf000, .patter = 0x0000, .opcode = "0nnn - SYS addr", .execute = &Cpu::sys},
};


inline void (Cpu::*findInstruction(const uint16_t opcode))() {
    for (const auto &insn: instructions) {
        if ((opcode & insn.mask) == insn.patter)
            return insn.execute;
    }

    return nullptr;
}


#endif //CHIP_8_EMULATOR_CPUINSTRUCTIONS_H
