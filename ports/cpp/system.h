#ifndef SYSTEM_H
#define SYSTEM_H

#include <array>
#include <functional>
#include <stack>
#include <vector>
#include "constants.h"
#include "font.h"

using namespace std;

class System
{
    array<uint8_t, RAM_SIZE> ram;
    array<uint8_t, REGISTERS_SIZE> registers;
    array<uint8_t, REGISTERS_SIZE> registers_backup;
    uint16_t program_counter;
    uint16_t index;
    stack<uint16_t> _stack;
    uint8_t delay_timer;
    uint8_t sound_timer;
    array<array<bool, DISPLAY_HEIGHT>, DISPLAY_WIDTH> display;
    Font font;
    bool is_key_pressed;
    uint8_t pressed_key;

    uint16_t next_opcode();
    void cpu_tick();

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

public:
    explicit System(const vector<uint8_t>& rom_bytes);

    void iterate_display(const function<void(uint8_t, uint8_t, bool)>& callback) const;

    void vblank();

    void notify_on_key_down(uint8_t key);
    void notify_on_key_up(uint8_t key);
};

#endif
