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
}


void NoteV2::setDisabled(bool value){
    disabled = value;
}


bool NoteV2::getDisabled(){
    return disabled;
}

uint8_t NoteV2::getIndex(){
    return index;
}


uint8_t NoteV2::getStartTime(){
    return startTime;
}


uint8_t NoteV2::getEndTime(){
    return endTime;
}


uint8_t NoteV2::getPadId(){
    return padId;
}



void NoteV2::update(int time){
    if (disabled == false){
        // direct time comparing only with rounded time 
        // uncomment to verify multiple triggers with same time value
        // if(time == startTime) __debugPrint(time, true);
        if(time == startTime && pendingPlay == false){ 
            startPlay();
        }
        if(time == endTime && pendingPlay == true){
            endPlay();
        }
    }
}


uint8_t NoteV2::getOutput(){
    return output;
}


void NoteV2::startRecord(uint8_t time, uint8_t _index, uint8_t _padId){
    index = _index;
    padId = _padId;

    pendingRecord = true;
    startTime = time;
    startPlay();
}


void NoteV2::endRecord(int time){
    if(pendingRecord==false) return;

    pendingRecord = false;
    endTime = time;
    duration = endTime - startTime; //duration is negative if end gets interrupted from clock reset
    // ---> todo add duration in update and check on endTime if duration is true
    setDisabled(false);
    endPlay();
}


void NoteV2::startPlay(){
    pendingPlay = true;
    output = 1;
}


void NoteV2::endPlay(){
    pendingPlay = false;
    output = 0;
}
