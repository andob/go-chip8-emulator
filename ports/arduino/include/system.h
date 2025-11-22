#ifndef SYSTEM_H
#define SYSTEM_H
#include <stdint.h>
#include <stddef.h>
#include "constants.h"
#include "stack.h"

class System {
    uint8_t ram[ramSize];
    uint8_t registers[registersSize];
    uint8_t registersBackup[registersSize];
    uint16_t programCounter;
    uint16_t index;
    Stack stack;
    uint8_t delayTimer;
    uint8_t soundTimer;
    bool display[displayWidth][displayHeight];
    bool isKeyPressed;
    uint8_t pressedKey;

    uint16_t nextOpcode();
    void cpuTick();

    void cls();
    void ret();
    void jp1(uint16_t pointer);
    void call(uint16_t pointer);
    void se1(uint8_t i, uint8_t value);
    void sne1(uint8_t i, uint8_t value);
    void se2(uint8_t i, uint8_t j);
    void ld1(uint8_t i, uint8_t value);
    void add1(uint8_t i, uint8_t value);
    void ld2(uint8_t i, uint8_t j);
    void _or(uint8_t i, uint8_t j);
    void _and(uint8_t i, uint8_t j);
    void _xor(uint8_t i, uint8_t j);
    void add2(uint8_t i, uint8_t j);
    void sub(uint8_t i, uint8_t j);
    void subn(uint8_t i, uint8_t j);
    void shr(uint8_t i);
    void shl(uint8_t i);
    void sne2(uint8_t i, uint8_t j);
    void ld3(uint16_t value);
    void jp2(uint16_t pointer);
    void rnd(uint8_t i, uint8_t value);
    void draw(uint8_t ix, uint8_t iy, uint8_t height);
    void skp(uint8_t i);
    void sknp(uint8_t i);
    void ld4(uint8_t i);
    void ldk(uint8_t i);
    void lddt(uint8_t i);
    void ldst(uint8_t i);
    void add3(uint8_t i);
    void ldf(uint8_t i);
    void ldb(uint8_t i);
    void reg2mem(uint8_t amount);
    void mem2reg(uint8_t amount);
    void reg2backup(uint8_t amount);
    void backup2reg(uint8_t amount);
    static void nop(uint8_t opcode);

public:
    System(const uint8_t* romBytes, size_t romLength);

    bool getPixel(uint8_t x, uint8_t y) const;

    void vblank();

    void notifyOnKeyDown(uint8_t key);
    void notifyOnKeyUp(uint8_t key);
};

#endif
