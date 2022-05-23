#include "SaveHandler.h"


SaveHandler::SaveHandler() {
  setup();
}

void SaveHandler::setup(){

}

void SaveHandler::update(){

}


int SaveHandler::saveSequences(int addressIndex, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes){
    for(uint8_t i=0; i<numSequences; i++){
      SequenceV2* sequence = sequences[i];
      ustd::array<NoteV2*> noteArray = sequence->getNoteArray();
      for(uint8_t i=0; i<numNotes; i++){
        NoteV2 *n = noteArray[i];
        if(n->getDisabled()){
          EEPROM.write(addressIndex, uint8_t(0));
        }else{
          EEPROM.write(addressIndex, uint8_t(1));
        }
        addressIndex++;
        EEPROM.write(addressIndex, n->getIndex());
        addressIndex++;
        EEPROM.write(addressIndex, n->getPadId());
        addressIndex++;
        EEPROM.write(addressIndex, n->getStartTime());
        addressIndex++;
        EEPROM.write(addressIndex, n->getEndTime());
        addressIndex++;
      }
    }
    return addressIndex;
}


int SaveHandler::loadSequences(int addressIndex, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes){
    for(uint8_t i=0; i<numSequences; i++){
      SequenceV2* sequence = sequences[i];
      ustd::array<NoteV2*> noteArray = sequence->getNoteArray();
      bool hasSavedData = false;
      for(uint8_t i=0; i<numNotes; i++){
        if(EEPROM.read(addressIndex)==1){
          NoteV2 *n = noteArray[i];
          n->createFromValues(EEPROM.read(addressIndex+1),EEPROM.read(addressIndex+2),EEPROM.read(addressIndex+3),EEPROM.read(addressIndex+4));
          hasSavedData = true;
        }
        addressIndex+=5;
      }
      if(hasSavedData){
        sequence->setEnabled(true);
      }
    }
    return addressIndex;
}


Array<int, 12> SaveHandler::loadNotes(int addressIndex){
  Array<int, 12> notes;
  for (int i = 0; i < 12; i++){
    notes[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
    addressIndex += 2;
  }
  return notes;
}


int SaveHandler::saveNotes(int addressIndex, Array<int, 12>  notes){
    for(uint8_t i=0; i<12; i++){
      EEPROM.write(addressIndex, notes[i] >> 8);
      EEPROM.write(addressIndex + 1, notes[i] & 0xFF);
      addressIndex += 2;
    }
    return addressIndex;
}


// int SaveHandler::writeIntArrayIntoEEPROM(int addressIndex, Array<int, 12> data, int arraySize)
// {
//   for (int i = 0; i < arraySize; i++) 
//   {
//     EEPROM.write(addressIndex, data[i] >> 8);
//     EEPROM.write(addressIndex + 1, data[i] & 0xFF);
//     addressIndex += 2;
//   }
//   return addressIndex;
// }

// Array<int, 12> SaveHandler::readIntArrayFromEEPROM(int addressIndex, int arraySize)
// {
//     Array<int, 12> notes;
//     for (int i = 0; i < arraySize; i++)
//     {
//         notes[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
//         addressIndex += 2;
//     }
//     return notes;
// }