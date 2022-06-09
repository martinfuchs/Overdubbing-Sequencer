#ifndef SaveHandler_h
#define SaveHandler_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Array.h>
#include "SequenceV2.h"
#include <ustd_array.h>
#include "NoteV2.h"

#include "SdFat.h"


// SDCARD_SS_PIN is defined for the built-in SD on some boards.
const uint8_t SD_CS_PIN = 10;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(8)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

//------------------------------------------------------------------------------
// Store error strings in flash to save RAM.
#define error(s) sd.errorHalt(&Serial, F(s))

class SaveHandler
{
    public:
        SaveHandler();
        void update();
        void saveSequences(char* fileName, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes);
        void loadSequences(char* fileName, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes);

        int saveNotes(int address, Array<int, 12>  notes);
        Array<int, 12> loadNotes(int address);

    private:
        void setup();
        // int writeIntArrayIntoEEPROM(int address, Array<int, 12> data, int arraySize);
        // Array<int, 12> readIntArrayFromEEPROM(int address, int arraySize);

        SdFat32 sd;
        File32 file;
        char line[40];
};

#endif