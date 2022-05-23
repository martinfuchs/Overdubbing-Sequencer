#ifndef SaveHandler_h
#define SaveHandler_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Array.h>
#include "SequenceV2.h"
#include <ustd_array.h>
#include "NoteV2.h"


class SaveHandler
{
    public:
        SaveHandler();
        void update();
        int saveSequences(int addressIndex, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes);
        int loadSequences(int addressIndex, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes);

        int saveNotes(int address, Array<int, 12>  notes);
        Array<int, 12> loadNotes(int address);

    private:
        void setup();
        // int writeIntArrayIntoEEPROM(int address, Array<int, 12> data, int arraySize);
        // Array<int, 12> readIntArrayFromEEPROM(int address, int arraySize);
};

#endif