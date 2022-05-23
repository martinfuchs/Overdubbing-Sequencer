#include "NoteV2.h"


NoteV2::NoteV2() {
    setup();
}


void NoteV2::setup() {
    reset();
}

void NoteV2::reset() {
    index = 0;
    padId = 0;
    startTime = 0;
    endTime = 0;
    duration = 0;
    pendingRecord = false;
    pendingPlay = false;
    disabled = true;
    output = 0;
    lastPressureUpdateTime = 0;
    usePressure = false;
}

uint8_t NoteV2::getIndex(){
    return index;
}


uint8_t NoteV2::getPadId(){
    return padId;
}


uint8_t NoteV2::getStartTime(){
    return startTime;
}


uint8_t NoteV2::getEndTime(){
    return endTime;
}


void NoteV2::setDisabled(bool value){
    disabled = value;
}


bool NoteV2::getDisabled(){
    return disabled;
}


bool NoteV2::isPlaying(){
    return pendingPlay;
}


void NoteV2::setPressureNote(PressureNote * _pNote){
    // could not include array into this class because of memory limit 
    // -> todo: find way to add longer pressure recordings
    pNote = _pNote;
    usePressure = true;
}


void NoteV2::recordPressure(uint32_t roundedTime, uint8_t pressure){
    if(usePressure == false) return;

    if (roundedTime != lastPressureUpdateTime && pNote->recordPosition<=pNote->maxLength-1){
        pNote->pressure[pNote->recordPosition] = pressure;
        pNote->recordPosition += 1;
        lastPressureUpdateTime = roundedTime;
    }
}

void NoteV2::playPressure(uint32_t roundedTime){    
    if (roundedTime != lastPressureUpdateTime && pNote->playbackPosition<=pNote->maxLength-1){
        output = pNote->pressure[pNote->playbackPosition];
        pNote->playbackPosition += 1;
        lastPressureUpdateTime = roundedTime;
    }
}


void NoteV2::update(uint32_t roundedTime){
    if (disabled == false){
        // direct time comparing only with rounded time! (millis*0.1)
        // uncomment to verify multiple triggers with same time value
        // if(roundedTime == startTime) __debugPrint(time, true);
        if(roundedTime == startTime && pendingPlay == false){ 
            startPlay();
        }
        if(roundedTime == endTime && pendingPlay == true){
            endPlay();
        }
        if(pendingPlay && usePressure){
            playPressure(roundedTime);
        }
    }
}


void NoteV2::createFromValues(uint8_t _index, uint8_t _padId, uint32_t _startTime, uint32_t _endTime){
    reset();
    setDisabled(false);
    index = _index;
    padId = _padId;
    startTime = _startTime;
    endTime = _endTime;
}


uint8_t NoteV2::getOutput(){
    return output;
}


void NoteV2::startRecord(uint32_t roundedTime, uint8_t _index, uint8_t _padId){
    index = _index;
    padId = _padId;
    if(usePressure) pNote->recordPosition = 0;

    pendingRecord = true;
    startTime = roundedTime;
    startPlay();
}


void NoteV2::endRecord(uint32_t roundedTime){
    if(pendingRecord==false) return;

    pendingRecord = false;
    endTime = roundedTime;
    duration = endTime - startTime; //duration is negative if end gets interrupted from clock reset
    // ---> todo: add duration in update and check on endTime if duration is true
    setDisabled(false);
    endPlay();

    //
    lastPressureUpdateTime = roundedTime;
}


void NoteV2::startPlay(){
    pendingPlay = true;
    if(usePressure==false) output = 255;
    if(usePressure) pNote->playbackPosition = 0;
}


void NoteV2::endPlay(){
    pendingPlay = false;
    output = 0;
}
