#include "SequenceV2.h"


SequenceV2::SequenceV2() {
}


void SequenceV2::setup(bool _printDebug ){
    setupSequenceNotes();
    setupFrameDisplay();
    printDebug = _printDebug;
}


void SequenceV2::setupSequenceNotes(){
    // create empty notes
    for(uint8_t i=0; i<MAX_NOTES; i++){
        NoteV2* tmpNote = new NoteV2();
        tmpNote->setDisabled(true);
        sequence[i] = tmpNote;
    }

    for(uint8_t i=0; i<NUM_TOUCHPADS; i++){
        NoteV2* tmpNote = new NoteV2();
        tmpNote->setDisabled(true);
        pendingInputNotes[i] = tmpNote;
    }
}


void SequenceV2::update(){
    if(playing==false || enabled==false){
        sequenceOutputValue = 0.0;
        return;
    }

    unsigned long currentTime = millis();
    unsigned long time = currentTime-startTime;
    roundedTime = int(time*0.1);

    sequenceOutputValue = 0.0;
    for(uint8_t i=0; i<MAX_NOTES; i++){
        sequence[i]->update(roundedTime);
        if (sequence[i]->getDisabled() == false && sequence[i]->getOutput()>=sequenceOutputValue){
            sequenceOutputValue = sequence[i]->getOutput();
            sequenceOutputNoteValue = getNoteValue(sequence[i]->getPadId());
        }
    }
}


int SequenceV2::getOutputValue(){
    float finalOutputValue = sequenceOutputValue;
    if(liveOutputValue>sequenceOutputValue) finalOutputValue = liveOutputValue;
    return finalOutputValue;
}


void SequenceV2::setTouchInputs(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record){
    recording = record;
    handsOn = false;
    for (uint8_t i=0; i<12; i++) {
        if(touches[i]->eventType == TouchPads::TOUCH_PRESSED){
            startLiveOutput(touches[i]);
            if(recording) startNoteRecord(touches[i]);
            handsOn = true;
        }
        if(touches[i]->eventType == TouchPads::TOUCH_RELEASED){
            endLiveOutput(touches[i]);
            if(recording) endNoteRecord(touches[i]);
        }
        if(touches[i]->eventType == TouchPads::TOUCH_PENDING){
            updateLiveOutput(touches[i]);
            if(recording) updateNoteRecord(touches[i]);
            handsOn = true;
        }
    }
}


void SequenceV2::stopPendingInputs(){
    for(uint8_t i=0; i<NUM_TOUCHPADS; i++){
        NoteV2* pending = pendingInputNotes[i];
        pending->endRecord(roundedTime);
    }
}


void SequenceV2::startNoteRecord(TouchPads::Touch* touch){
    __debugPrint("Start note record");
    NoteV2* newNote = sequence[currentNoteIndex];
    Serial.println(currentNoteIndex);
    newNote->reset();
    newNote->startRecord(roundedTime, currentNoteIndex, touch->touchPadId, printDebug);
    pendingInputNotes[touch->touchPadId] = newNote;

    if(currentNoteIndex<=MAX_NOTES-2){
        currentNoteIndex +=1;
    }
}


void SequenceV2::endNoteRecord(TouchPads::Touch* touch){
    NoteV2* pending = pendingInputNotes[touch->touchPadId];
    pending->endRecord(roundedTime);
    __debugPrint("End note record");
}


void SequenceV2::updateNoteRecord(TouchPads::Touch* touch){

}


void SequenceV2::startLiveOutput(TouchPads::Touch* touch){
    liveOutputValue = 1.0;
    liveOutputNoteValue = getNoteValue(touch->touchPadId);
}

void SequenceV2::updateLiveOutput(TouchPads::Touch* touch){
    //if(usePressureValues) liveOutputValue = touch.pressure;
}

void SequenceV2::endLiveOutput(TouchPads::Touch* touch){
    liveOutputValue = 0.0;
}


void SequenceV2::restart(){
    __debugPrint("reset");
    startTime = millis();
}


void SequenceV2::play(){
    playing = true;
}


void SequenceV2::stop(){
     playing = false;
}

void SequenceV2::setEnabled(bool value){
    enabled = value;
}

bool SequenceV2::getEnabled(){
    return enabled;
}


void SequenceV2::clearAll(){
    currentNoteIndex = 0;
    for(uint8_t i=0; i<MAX_NOTES; i++){
        NoteV2 *n = sequence[i];
        n->setDisabled(true);
    }
}


void SequenceV2::undo(){
    currentNoteIndex-=1;
    if(currentNoteIndex<0){
        currentNoteIndex = 0;
    }
    
    NoteV2* n = sequence[currentNoteIndex];
    n->setDisabled(true);
    __debugPrint("undo");
}


void SequenceV2::setNoteValueRef(NoteValues *nvalues){
    notevalues = nvalues;
    hasNoteValues = true;
}


int SequenceV2::getNoteValue(int padId){
    if(hasNoteValues==false){
        return 65535; // return lowest value
    }

    //Array<int, 12> nArray = notevalues->getNoteValues();
    // Serial.print("New Note ID: ");
    // Serial.println(padId);
    //return nArray[padId];
    return notevalues->getNoteValues()[padId];
}


int SequenceV2::getActiveNoteValue(){
    if(liveOutputValue>0){
        return liveOutputNoteValue;
    }else{
        return sequenceOutputNoteValue;
    }
}


void SequenceV2::setupFrameDisplay(){
    frame = new int*[4];
    for (uint8_t i = 0; i < 4; i++){
        frame[i] = new int[5];
    }
}


int** SequenceV2::getFrameDisplay(){
    for (int y=0; y<4; y++){
        for (int x=0; x<5; x++){
            frame[y][x] = 0;
        }
    }

    uint8_t x = 0;
    uint8_t y = 0;
    for(uint8_t i=0; i<MAX_NOTES; i++){
        NoteV2 *n = sequence[i];
        if(i<MAX_NOTES){
            if(roundedTime>n->getStartTime() && roundedTime<n->getEndTime()){
                frame[y][x] = 0;
            }else{
                frame[y][x] = 1;
            }
            if(n->getDisabled()==true){
                frame[y][x] = 0;
            }

            x++;
            if(x>4){
                 x=0;
                y++;
            }
        }else{
            return frame;
        }
    }
    return frame;
}


void SequenceV2::__debugPrint(String message){
    if(printDebug==false) return;
    Serial.println(message);
}