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
    array<u8, RAM_SIZE> ram;
    array<u8, REGISTERS_SIZE> registers;
    array<u8, REGISTERS_SIZE> registers_backup;
    u16 program_counter;
    u16 index;
    stack<u16> _stack;
    u8 delay_timer;
    u8 sound_timer;
    array<array<bool, DISPLAY_HEIGHT>, DISPLAY_WIDTH> display;
    Font font;
    bool is_key_pressed;
    u8 pressed_key;

    u16 next_opcode();
    void cpu_tick();

    void cls();
    void ret();
    void jp1(u16 pointer);
    void call(u16 pointer);
    void se1(u8 i, u8 value);
    void sne1(u8 i, u8 value);
    void se2(u8 i, u8 j);
    void ld1(u8 i, u8 value);
    void add1(u8 i, u8 value);
    void ld2(u8 i, u8 j);
    void _or(u8 i, u8 j);
    void _and(u8 i, u8 j);
    void _xor(u8 i, u8 j);
    void add2(u8 i, u8 j);
    void sub(u8 i, u8 j);
    void subn(u8 i, u8 j);
    void shr(u8 i);
    void shl(u8 i);
    void sne2(u8 i, u8 j);
    void ld3(u16 value);
    void jp2(u16 pointer);
    void rnd(u8 i, u8 value);
    void draw(u8 ix, u8 iy, u8 height);
    void skp(u8 i);
    void sknp(u8 i);
    void ld4(u8 i);
    void ldk(u8 i);
    void lddt(u8 i);
    void ldst(u8 i);
    void add3(u8 i);
    void ldf(u8 i);
    void ldb(u8 i);
    void reg2mem(u8 amount);
    void mem2reg(u8 amount);
    void reg2backup(u8 amount);
    void backup2reg(u8 amount);

public:
    explicit System(const vector<u8>& rom_bytes);

    void iterate_display(const function<void(u8, u8, bool)>& callback) const;

    void vblank();

    void notify_on_key_down(u8 key);
    void notify_on_key_up(u8 key);
};

#endif //SYSTEM_H
