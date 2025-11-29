#include "frontend.h"

#define xOffset 8
#define yOffset 8
#define blockWidth 5
#define blockHeight 7

void Frontend::renderFrame() const {
    for (uint16_t x = 0; x < displayWidth; x++) {
        for (uint16_t y = 0; y < displayHeight; y++) {
            if (system->wasPixelChanged(x, y)) {
                const uint16_t projectedX = xOffset + x * blockWidth;
                const uint16_t projectedY = yOffset + y * blockHeight;
                const uint16_t color = system->getPixel(x, y) ? ILI9341_GREENYELLOW : ILI9341_BLACK;
                touchscreen->fillRect(projectedX, projectedY, blockWidth, blockHeight, color);
            }
        }
    }
}
