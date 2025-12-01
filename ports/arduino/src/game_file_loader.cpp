#include "game_file_loader.h"

void GameFileLoader::initializeIfNeeded() {
    if (!isSDInitialized) {
        SD.begin(sdCardReaderChipSelectPin);
        isSDInitialized = true;
    }
}

bool GameFileLoader::isFileAGame(File& file) {
    const char* fileName = file.name();
    const uint16_t fileNameLength = strlen(fileName);
    return fileNameLength > 4 && fileName[0] != '_' &&
        strcasecmp(fileName + (fileNameLength - 4), ".ch8") == 0;
}


uint16_t GameFileLoader::getNumberOfGameFiles() {
    initializeIfNeeded();

    uint16_t numberOfGames = 0;
    File directory = SD.open(sdCardBasePath);
    File file = directory.openNextFile();
    while (file) {
        if (isFileAGame(file)) {
            numberOfGames++;
        }

        file.close();
        file = directory.openNextFile();
    }

    directory.close();
    return numberOfGames;
}

File GameFileLoader::getNthGameFile(const uint16_t targetFileIndex) {
    initializeIfNeeded();

    uint16_t currentFileIndex = 0;
    File directory = SD.open(sdCardBasePath);
    File file = directory.openNextFile();
    while (file) {
        if (isFileAGame(file)) {
            if (currentFileIndex == targetFileIndex) {
                directory.close();
                return file;
            }

            currentFileIndex++;
        }

        file.close();
        file = directory.openNextFile();
    }

    directory.close();
    return File();
}

File GameFileLoader::getRandomGameFile() {
    initializeIfNeeded();

    const uint16_t numberOfGames = getNumberOfGameFiles();
    const uint16_t gameIndex = random(0, numberOfGames);
    return getNthGameFile(gameIndex);
}
