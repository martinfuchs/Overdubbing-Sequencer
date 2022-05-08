#include "SequenceV2.h"


SequenceV2::SequenceV2() {
}


///////////////////
// INIT 
///////////////////

void SequenceV2::setup(uint8_t _numNotes, bool _usePressureValues, bool _printDebug){
    numNotes = _numNotes;
    printDebug = _printDebug;
    usePressureValues = _usePressureValues;

    setupSequenceNotes();
    setupFrameDisplay();
}


void SequenceV2::setupSequenceNotes(){
    // create empty notes
    for(uint8_t i=0; i<numNotes; i++){
        NoteV2* tmpNote = new NoteV2();
        tmpNote->setDisabled(true);
        //sequence[i] = tmpNote;
        sequence.add(tmpNote);
    }

    for(uint8_t i=0; i<NUM_TOUCHPADS; i++){
        NoteV2* tmpNote = new NoteV2();
        tmpNote->setDisabled(true);
        pendingInputNotes[i] = tmpNote;
    }

    if(usePressureValues){
        pressureNotes = ustd::map<uint8_t, PressureNote*>(numNotes, numNotes, 0, false);
        for(uint8_t i=0; i<numNotes; i++){
            PressureNote* pNote = new PressureNote();
            pressureNotes[i] = pNote;
            sequence[i]->setPressureNote(pNote);
        }
    }
}


///////////////////
// OUTPUT 
///////////////////

uint8_t SequenceV2::getOutputValue(){
    uint8_t finalOutputValue = sequenceOutputValue;
    if(liveOutputValue>sequenceOutputValue) finalOutputValue = liveOutputValue;

    if(usePressureValues){
        interpolatedOutputValue += (float(finalOutputValue)-interpolatedOutputValue)*0.05;
        return uint8_t(interpolatedOutputValue);
    }
    return finalOutputValue;
}


///////////////////
// INPUT 
///////////////////

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


///////////////////
// SEQUENCE 
///////////////////

void SequenceV2::update(){
    if(playing==false || enabled==false){
        sequenceOutputValue = 0;
        return;
    }

    unsigned long currentTime = millis();
    unsigned long time = currentTime-startTime;
    roundedTime = uint32_t(time*0.1);

    sequenceOutputValue = 0;
    for(uint8_t i=0; i<numNotes; i++){
        sequence[i]->update(roundedTime);
        if (sequence[i]->getDisabled() == false && sequence[i]->getOutput()>=sequenceOutputValue){
            sequenceOutputValue = sequence[i]->getOutput();
            sequenceOutputNoteValue = getNoteValue(sequence[i]->getPadId());
        }
    }
}


void SequenceV2::startNoteRecord(TouchPads::Touch* touch){
    __debugPrint("Start note record");
    NoteV2* newNote = sequence[currentNoteIndex];
    newNote->reset();
    newNote->startRecord(roundedTime, currentNoteIndex, touch->touchPadId);
    pendingInputNotes[touch->touchPadId] = newNote;

    if(currentNoteIndex<=numNotes-2){
        currentNoteIndex +=1;
    }
}


void SequenceV2::endNoteRecord(TouchPads::Touch* touch){
    __debugPrint("End note record");
    NoteV2* pending = pendingInputNotes[touch->touchPadId];
    pending->endRecord(roundedTime);
}


void SequenceV2::updateNoteRecord(TouchPads::Touch* touch){
    if(usePressureValues){
        NoteV2* pending = pendingInputNotes[touch->touchPadId];
        pending->recordPressure(roundedTime, touch->pressure);
    }
}


///////////////////
// LIVE 
///////////////////

void SequenceV2::startLiveOutput(TouchPads::Touch* touch){
    liveOutputValue = 255;
    liveOutputNoteValue = getNoteValue(touch->touchPadId);
}

void SequenceV2::updateLiveOutput(TouchPads::Touch* touch){
    if(usePressureValues) liveOutputValue = touch->pressure;
}

void SequenceV2::endLiveOutput(TouchPads::Touch* touch){
    liveOutputValue = 0;
}


///////////////////
// CONTROL
///////////////////

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
    for(uint8_t i=0; i<numNotes; i++){
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


///////////////////
// NOTEVALUES
///////////////////

void SequenceV2::setNoteValueRef(NoteValues *nvalues){
    notevalues = nvalues;
    hasNoteValues = true;
}


uint16_t SequenceV2::getNoteValue(uint8_t padId){
    if(hasNoteValues==false){
        return 65535; // return lowest value
    }
    return notevalues->getNoteValues()[padId];
}


uint16_t SequenceV2::getActiveNoteValue(){
    if(liveOutputValue>0){
        return liveOutputNoteValue;
    }else{
        return sequenceOutputNoteValue;
    }
}


///////////////////
// DISPLAY
///////////////////

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
    for(uint8_t i=0; i<MAX_NOTES_DISPLAY; i++){
        NoteV2 *n = sequence[i];
        if(i<numNotes){
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