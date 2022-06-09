#include "SaveHandler.h"


SaveHandler::SaveHandler() {
  setup();
}

void SaveHandler::setup(){

}

void SaveHandler::update(){

}


void SaveHandler::saveSequences(char* fileName, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes){

    // Initialize the SD.
    if (!sd.begin(SD_CONFIG)) {
      sd.initErrorHalt(&Serial);
      return;
    }

    // Remove any existing file.
    if (sd.exists(fileName)) {
      sd.remove(fileName);
    }
    // Create the file.
    if (!file.open(fileName, FILE_WRITE)) {
      Serial.println("open failed");
    }

    // Write data
    file.print("+CHANNEL:"+String(fileName)+",0,0,0\r\n");
    for(uint8_t i=0; i<numSequences; i++){
      SequenceV2* sequence = sequences[i];
      file.print("#SEQ:"+String(i)+",0,0,0\r\n");
      ustd::array<NoteV2*> noteArray = sequence->getNoteArray();
      for(uint8_t i=0; i<numNotes; i++){
        NoteV2 *n = noteArray[i];
        String saveString = "-1,0,0,0";
        if(n->getDisabled()==false){
          // uint8_t, uint8_t, uint32_t, uint32_t
          saveString = String(n->getIndex())+","+String(n->getPadId())+","+String(n->getStartTime())+","+String(n->getEndTime());
        }
        file.print(saveString+"\r\n");
      }
    }
    file.close();
    Serial.println("Done");
}

bool equalString(String a, String b){
  if(a == b){
    return true;
  }
  return false;
}

bool firstChar(String line, String first){
  return equalString(line.charAt(0),first);
}


void SaveHandler::loadSequences(char* fileName, ustd::array<SequenceV2*> sequences, uint8_t numSequences, uint8_t numNotes){
    Serial.print("Load File:");
    Serial.println(fileName);

    // Initialize the SD.
    if (!sd.begin(SD_CONFIG)) {
      sd.initErrorHalt(&Serial);
      return;
    }

    // Create the file.
    if (!file.open(fileName, FILE_WRITE)) {
      Serial.println("open failed");
    }

    // Rewind file for read.
    file.rewind();

    int sequenceCount = 0;
    int noteCount = 0;

    while (file.available()) {
      int n = file.fgets(line, sizeof(line));
      if (n <= 0) {
        Serial.println("fgets failed");
      }
      if (line[n-1] != '\n' && n == (sizeof(line) - 1)) {
        Serial.println("line too long");
      }

      
      // Parse line
      int nIndex = -1;
      uint8_t nPadId = 0;
      uint32_t nStartTime = 0;
      uint32_t nEndTime = 0;
      //
      char *p = line;
      char *str;
      int charIndex = 0;
      while ((str = strtok_r(p, ",", &p)) != NULL){
        //Serial.print(charIndex);
        //Serial.print(": ");
        //Serial.println(str);
        //
        if(charIndex==0){
          if(firstChar(str,"-")){
            //Serial.print("-- EMPTY NOTE:");
            //Serial.println(noteCount);
            nIndex = -1;
            noteCount++;
          }else if(firstChar(str,"#")){
            //Serial.print("------------- SEQUENCE:");
            //Serial.println(sequenceCount);
            sequenceCount+=1;
            noteCount = 0;
            nIndex = -1;
          }else if(firstChar(str,"+")){
            //Serial.println("------------- HEADER");
            nIndex = -1;
          }else{
            // Serial.print("-- NOTE:");
            // Serial.println(noteCount);
            nIndex = String(str).toInt();
            noteCount++;
          }
        }
        if(nIndex!=-1){
          if(charIndex==1){
            nPadId = String(str).toInt();
          }else if(charIndex==2){
            nStartTime = String(str).toInt();
          }else if(charIndex==3){
            nEndTime = String(str).toInt();
          }
        }
        charIndex+=1;
      }

      // Apply data
      if(nIndex!=-1){
        int seqCount = sequenceCount-1;
        int nCount = noteCount-1;
        if(seqCount<numSequences){
          Serial.println("+++++++++++++++++++");
          Serial.print("SEQ:");
          Serial.println(seqCount);

          Serial.print("NOTE:");
          Serial.println(nCount);
          Serial.println(nIndex);
          Serial.println(nPadId);
          Serial.println(nStartTime);
          Serial.println(nEndTime);
          Serial.println("+++++++++++++++++++");
          Serial.println();

          SequenceV2* sequence = sequences[seqCount];
          ustd::array<NoteV2*> noteArray = sequence->getNoteArray();
          NoteV2 *n = noteArray[nCount];
          n->createFromValues(nIndex,nPadId,nStartTime,nEndTime);
          sequence->increaseCurrentNoteIndex();
          sequence->setEnabled(true);
          sequence->play();
        }
      }

    }
    file.close();
    Serial.println(F("Done"));
}



Array<int, 12> SaveHandler::loadNotes(int addressIndex){
  Array<int, 12> notes;
  for (int i = 0; i < 12; i++){
    notes[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
    addressIndex += 2;
  }
  return notes;
}

//Teensy 3.1	2048 bytes EEPROM
int SaveHandler::saveNotes(int addressIndex, Array<int, 12>  notes){
    for(uint8_t i=0; i<12; i++){
      EEPROM.write(addressIndex, notes[i] >> 8);
      EEPROM.write(addressIndex + 1, notes[i] & 0xFF);
      addressIndex += 2;
    }
    return addressIndex;
}