#ifndef GAME_FILE_LOADER
#define GAME_FILE_LOADER
#include <stdint.h>
#include "SD.h"

class GameFileLoader {
private:
    bool isSDInitialized = false;
    uint8_t sdCardReaderChipSelectPin;
    const char* sdCardBasePath;

    void initializeIfNeeded();
    static bool isFileAGame(File& file);
    uint16_t getNumberOfGameFiles();
    File getNthGameFile(const uint16_t targetFileIndex);

public:
    GameFileLoader(const uint8_t pin, const char* basePath)
        : sdCardReaderChipSelectPin(pin), sdCardBasePath(basePath) {}

    File getRandomGameFile();
};

#endif
