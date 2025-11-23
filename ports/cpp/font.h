#ifndef FONT_H
#define FONT_H

#include <array>
#include "constants.h"

#define NOF_BYTES_IN_FONT_CHAR 5
#define NOF_CHARS_IN_FONT 16

using namespace std;

class FontCharacter
{
    uint8_t name;
    uint16_t _address;
    array<uint8_t, NOF_BYTES_IN_FONT_CHAR> data;

public:
    FontCharacter(const uint8_t name, const uint16_t address, const array<uint8_t, NOF_BYTES_IN_FONT_CHAR> &data)
        : name(name), _address(address), data(data) {}

    uint8_t get_name() const { return name; }
    uint16_t get_address() const { return _address; }
    array<uint8_t, NOF_BYTES_IN_FONT_CHAR> get_data() const { return data; }
};

class Font
{
    array<FontCharacter, NOF_CHARS_IN_FONT> chars;

public:
    explicit Font(const array<FontCharacter, NOF_CHARS_IN_FONT> &chars) : chars(chars) {}

    array<FontCharacter, NOF_CHARS_IN_FONT> get_characters() const { return chars; }

    static Font get_default();
};

#endif
