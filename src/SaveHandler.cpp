#include "SaveHandler.h"


SaveHandler::SaveHandler() {
  setup();
}

void SaveHandler::setup(){

}

void SaveHandler::update(){

}

Array<int, 12> SaveHandler::loadNotes(){
    return readIntArrayFromEEPROM(0, 12);
}

void SaveHandler::saveNotes(Array<int, 12> notes){
    writeIntArrayIntoEEPROM(0,notes,12);
}


void SaveHandler::writeIntArrayIntoEEPROM(int address, Array<int, 12> data, int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++) 
  {
    EEPROM.write(addressIndex, data[i] >> 8);
    EEPROM.write(addressIndex + 1, data[i] & 0xFF);
    addressIndex += 2;
  }
}

Array<int, 12> SaveHandler::readIntArrayFromEEPROM(int address, int arraySize)
{
    Array<int, 12> notes;
    int addressIndex = address;
    for (int i = 0; i < arraySize; i++)
    {
        notes[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
        addressIndex += 2;
    }
    return notes;
}