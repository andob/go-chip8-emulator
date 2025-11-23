#include "font.h"
#include "constants.h"

using namespace std;

Font Font::get_default()
{
    return Font({
        // ReSharper disable CppFunctionalStyleCast
        FontCharacter(uint8_t(0x0), uint16_t(0x050), { uint8_t(0xF0), uint8_t(0x90), uint8_t(0x90), uint8_t(0x90), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x1), uint16_t(0x055), { uint8_t(0x20), uint8_t(0x60), uint8_t(0x20), uint8_t(0x20), uint8_t(0x70) }),
        FontCharacter(uint8_t(0x2), uint16_t(0x05A), { uint8_t(0xF0), uint8_t(0x10), uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x3), uint16_t(0x05F), { uint8_t(0xF0), uint8_t(0x10), uint8_t(0xF0), uint8_t(0x10), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x4), uint16_t(0x064), { uint8_t(0x90), uint8_t(0x90), uint8_t(0xF0), uint8_t(0x10), uint8_t(0x10) }),
        FontCharacter(uint8_t(0x5), uint16_t(0x069), { uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0), uint8_t(0x10), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x6), uint16_t(0x06E), { uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0), uint8_t(0x90), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x7), uint16_t(0x073), { uint8_t(0xF0), uint8_t(0x10), uint8_t(0x20), uint8_t(0x40), uint8_t(0x40) }),
        FontCharacter(uint8_t(0x8), uint16_t(0x078), { uint8_t(0xF0), uint8_t(0x90), uint8_t(0xF0), uint8_t(0x90), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0x9), uint16_t(0x07D), { uint8_t(0xF0), uint8_t(0x90), uint8_t(0xF0), uint8_t(0x10), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0xA), uint16_t(0x082), { uint8_t(0xF0), uint8_t(0x90), uint8_t(0xF0), uint8_t(0x90), uint8_t(0x90) }),
        FontCharacter(uint8_t(0xB), uint16_t(0x087), { uint8_t(0xE0), uint8_t(0x90), uint8_t(0xE0), uint8_t(0x90), uint8_t(0xE0) }),
        FontCharacter(uint8_t(0xC), uint16_t(0x08C), { uint8_t(0xF0), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0xD), uint16_t(0x091), { uint8_t(0xE0), uint8_t(0x90), uint8_t(0x90), uint8_t(0x90), uint8_t(0xE0) }),
        FontCharacter(uint8_t(0xE), uint16_t(0x096), { uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0) }),
        FontCharacter(uint8_t(0xF), uint16_t(0x09B), { uint8_t(0xF0), uint8_t(0x80), uint8_t(0xF0), uint8_t(0x80), uint8_t(0x80) }),
    });
}
