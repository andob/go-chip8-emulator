#include <Arduino.h>
#include "Adafruit_ILI9341.h"
#include "emulator/system.h"
#include "game_file_loader.h"
#include "frontend.h"
#include "joystick.h"

#define baudRate 9600

#define enableRAMUsageLogging
#undef enableRAMUsageLogging
extern int __heap_start, *__brkval;
int getFreeRAM() { int v; return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval); }

#define touchscreenDataPin 9
#define touchscreenChipSelectPin 10
#define touchscreenRotation 1
Adafruit_ILI9341 touchscreen = Adafruit_ILI9341(touchscreenChipSelectPin, touchscreenDataPin);

#define sdCardBasePath "/"
#define sdCardReaderChipSelectPin 4
GameFileLoader gameFileLoader(sdCardReaderChipSelectPin, sdCardBasePath);

#define defaultGameSize 340
const uint8_t defaultGameBytes[defaultGameSize] PROGMEM = {
     0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f, 0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x38, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff, 0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b, 0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0xbf, 0x00, 0xfb, 0x00, 0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00, 0xe0
};

System chip8System(defaultGameBytes, defaultGameSize);
Frontend frontend(&chip8System, &touchscreen);

#define joystickButtonPin 53
#define joystickXAxisPin A14
#define joystickYAxisPin A15
Joystick joystick(joystickButtonPin, joystickXAxisPin, joystickYAxisPin);

class JoystickCallbacksImpl final : public JoystickCallbacks {
public:
     void onButtonPressed() override { chip8System.notifyOnKeyDown(key5Code); }
     void onButtonReleased() override { chip8System.notifyOnKeyUp(key5Code); }
     void onUpPressed() override { chip8System.notifyOnKeyDown(key2Code); }
     void onUpReleased() override { chip8System.notifyOnKeyUp(key2Code); }
     void onDownPressed() override { chip8System.notifyOnKeyDown(key8Code); }
     void onDownReleased() override { chip8System.notifyOnKeyUp(key8Code); }
     void onLeftPressed() override { chip8System.notifyOnKeyDown(key4Code); }
     void onLeftReleased() override  { chip8System.notifyOnKeyUp(key4Code); }
     void onRightPressed() override { chip8System.notifyOnKeyDown(key6Code); }
     void onRightReleased() override { chip8System.notifyOnKeyUp(key6Code); }
} joystickCallbacks;

uint64_t generateRandomSeed() {
     uint64_t seed = 0;
     for (uint8_t analogPin = A8; analogPin <= A13; analogPin++) {
          seed = seed * 31 + analogRead(analogPin);
     }

     seed = seed * 31 + micros();
     return seed;
}

void setup() {
     Serial.begin(baudRate);

     randomSeed(generateRandomSeed());

     touchscreen.begin();
     touchscreen.setRotation(touchscreenRotation);
     touchscreen.fillScreen(ILI9341_BLACK);

     File gameFile = gameFileLoader.getRandomGameFile();
     if (gameFile.size() > 0) {
          chip8System.loadGameFile(gameFile);
     }

     joystick.setCallbacks(&joystickCallbacks);
}

void loop() {
     frontend.renderFrame();

     joystick.check();

     chip8System.vblank();

     #ifdef enableRAMUsageLogging
     Serial.print("Free RAM: ");
     Serial.println(getFreeRAM());
     #endif
}
