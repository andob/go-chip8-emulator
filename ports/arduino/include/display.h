#ifndef DISPLAY_H
#define DISPLAY_H
#include "constants.h"
#include <stdint.h>

class Display {
private:
    uint8_t pixels[displayWidth][displayHeight / nofBitsInByte];

public:
    Display() = default;

    bool getPixel(uint8_t x, uint8_t y) const;
    void setPixel(uint8_t x, uint8_t y, bool value);
};

#endif
