#ifndef FONT_H
#define FONT_H

#include <array>
#include "constants.h"

#define NOF_BYTES_IN_FONT_CHAR 5
#define NOF_CHARS_IN_FONT 16

using namespace std;

class FontCharacter
{
    u8 name;
    u16 _address;
    array<u8, NOF_BYTES_IN_FONT_CHAR> data;

public:
    FontCharacter(const u8 name, const u16 address, const array<u8, NOF_BYTES_IN_FONT_CHAR> &data)
        : name(name), _address(address), data(data) {}

    u8 get_name() const { return name; }
    u16 get_address() const { return _address; }
    array<u8, NOF_BYTES_IN_FONT_CHAR> get_data() const { return data; }
};

class Font
{
    array<FontCharacter, NOF_CHARS_IN_FONT> chars;

public:
    explicit Font(const array<FontCharacter, NOF_CHARS_IN_FONT> &chars) : chars(chars) {}

    array<FontCharacter, NOF_CHARS_IN_FONT> get_characters() const { return chars; }

    static Font get_default();
};

#endif //FONT_H
