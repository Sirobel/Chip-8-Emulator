//
// Created by finns on 24.10.26.
//

#ifndef CHIP_8_EMULATOR_CPU_H
#define CHIP_8_EMULATOR_CPU_H
#include <cstdint>



class Cpu {

public:
    typedef void (*cpuExecuteOp)();
    uint8_t memory[4096]{};

    struct Register {
        uint8_t V[16];
        uint16_t I;
        uint8_t delay;
        uint8_t soundTimers;
        uint16_t PC;
        uint8_t SP;
        uint16_t stack[16];
    };
    Register registers{};

    static constexpr int SCREEN_WIDTH = 64;
    static constexpr int SCREEN_HEIGHT = 32;

    uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT]{};

    // 5 bytes with per font with  16 fonts
    const uint8_t fontSet[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0xF0, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80 //F
    };

    Cpu();
    void cycle();

    void sys();
    void cls();
    void ret();;
    void jp();
    void call();
    void seVB();
    void sneVB();
    void seVV();
    void ldVB();
    void add();
    void ldVV();
    void orVV();
    void andVV();
    void xorVV();
    void addVV();
    void subVV();
    void shrV();
    void subnVV();
    void shlV();
    void sneVV();
    void LdI();
    void jpV();
    void rnd();
    void drw();
    void skp();
    void addIV();
    void ldBV();
    void ldIV();
    void ldVI();
private:
    uint16_t currentOpcode = 0;
    void (Cpu::*currentInstruction)() = nullptr;

    void initiateFontSet();

    void initiate();

    void fetchOpcode();

    void executeOpcode();
};


#endif //CHIP_8_EMULATOR_CPU_H
