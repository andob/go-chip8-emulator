#ifndef FRONTEND_H
#define FRONTEND_H
#include "emulator/system.h"
#include "Adafruit_ILI9341.h"

class Frontend
{
private:
    System* system;
    Adafruit_ILI9341* touchscreen;

public:
    explicit Frontend(System* system, Adafruit_ILI9341* touchscreen) : system(system), touchscreen(touchscreen) {}

    void renderFrame() const;
};

#endif
