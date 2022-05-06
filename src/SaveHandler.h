#ifndef SaveHandler_h
#define SaveHandler_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Array.h>


class SaveHandler
{
    public:
        SaveHandler();
        void update();
        //void saveSequenceMaps()
        void saveNotes(Array<int, 12> notes);
        Array<int, 12> loadNotes();

    private:
        void setup();
        void writeIntArrayIntoEEPROM(int address, Array<int, 12> data, int arraySize);
        Array<int, 12> readIntArrayFromEEPROM(int address, int arraySize);
};

#endif