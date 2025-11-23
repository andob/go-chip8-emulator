#include "display.h"

bool Display::getPixel(const uint8_t x, const uint8_t y) const {
    if (x < displayWidth && y < displayHeight) {
        const uint8_t data = pixels[x][y / nofBitsInByte];
        const uint8_t bitIndex = y % nofBitsInByte;
        return (data >> bitIndex) & 1;
    }

    return false;
}

void Display::setPixel(const uint8_t x, const uint8_t y, const bool value) {
    if (x < displayWidth && y < displayHeight) {
        const uint8_t data = pixels[x][y / nofBitsInByte];
        const uint8_t bitIndex = y % nofBitsInByte;
        const uint8_t newData = value ? data | (1 << bitIndex) : data & ~(1 << bitIndex);
        pixels[x][y / nofBitsInByte] = newData;
    }
}
