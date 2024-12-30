#include "font.h"
#include "constants.h"

using namespace std;

Font Font::get_default()
{
    return Font({
        // ReSharper disable CppFunctionalStyleCast
        FontCharacter(u8(0x0), u16(0x050), { u8(0xF0), u8(0x90), u8(0x90), u8(0x90), u8(0xF0) }),
        FontCharacter(u8(0x1), u16(0x055), { u8(0x20), u8(0x60), u8(0x20), u8(0x20), u8(0x70) }),
        FontCharacter(u8(0x2), u16(0x05A), { u8(0xF0), u8(0x10), u8(0xF0), u8(0x80), u8(0xF0) }),
        FontCharacter(u8(0x3), u16(0x05F), { u8(0xF0), u8(0x10), u8(0xF0), u8(0x10), u8(0xF0) }),
        FontCharacter(u8(0x4), u16(0x064), { u8(0x90), u8(0x90), u8(0xF0), u8(0x10), u8(0x10) }),
        FontCharacter(u8(0x5), u16(0x069), { u8(0xF0), u8(0x80), u8(0xF0), u8(0x10), u8(0xF0) }),
        FontCharacter(u8(0x6), u16(0x06E), { u8(0xF0), u8(0x80), u8(0xF0), u8(0x90), u8(0xF0) }),
        FontCharacter(u8(0x7), u16(0x073), { u8(0xF0), u8(0x10), u8(0x20), u8(0x40), u8(0x40) }),
        FontCharacter(u8(0x8), u16(0x078), { u8(0xF0), u8(0x90), u8(0xF0), u8(0x90), u8(0xF0) }),
        FontCharacter(u8(0x9), u16(0x07D), { u8(0xF0), u8(0x90), u8(0xF0), u8(0x10), u8(0xF0) }),
        FontCharacter(u8(0xA), u16(0x082), { u8(0xF0), u8(0x90), u8(0xF0), u8(0x90), u8(0x90) }),
        FontCharacter(u8(0xB), u16(0x087), { u8(0xE0), u8(0x90), u8(0xE0), u8(0x90), u8(0xE0) }),
        FontCharacter(u8(0xC), u16(0x08C), { u8(0xF0), u8(0x80), u8(0x80), u8(0x80), u8(0xF0) }),
        FontCharacter(u8(0xD), u16(0x091), { u8(0xE0), u8(0x90), u8(0x90), u8(0x90), u8(0xE0) }),
        FontCharacter(u8(0xE), u16(0x096), { u8(0xF0), u8(0x80), u8(0xF0), u8(0x80), u8(0xF0) }),
        FontCharacter(u8(0xF), u16(0x09B), { u8(0xF0), u8(0x80), u8(0xF0), u8(0x80), u8(0x80) }),
    });
}