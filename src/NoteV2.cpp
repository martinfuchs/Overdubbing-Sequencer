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
    //endTime = 0;
    durationMillis = 0;
    pendingRecord = false;
    pendingPlay = false;
    disabled = true;
    output = 0;
    lastUpdateTimeMillis = 0;
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


// uint8_t NoteV2::getEndTime(){
//     return startTime+durationMillis*0.1; //*0.1 => rounded time
// }


uint16_t NoteV2::getDurationMillis(){
    return durationMillis;
}

void NoteV2::setDurationMillis(uint16_t _durationMillis){
    durationMillis = _durationMillis;
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

    // if (roundedTime != lastUpdateTime && pNote->recordPosition<=pNote->maxLength-1){
    //     pNote->pressure[pNote->recordPosition] = pressure;
    //     pNote->recordPosition += 1;
    // }
}

void NoteV2::playPressure(uint32_t roundedTime){    
    // if (roundedTime != lastUpdateTime && pNote->playbackPosition<=pNote->maxLength-1){
    //     output = pNote->pressure[pNote->playbackPosition];
    //     pNote->playbackPosition += 1;
    // }
}


void NoteV2::update(uint32_t roundedTime){

    uint32_t currentTime = millis();
    uint16_t diffTime = currentTime-lastUpdateTimeMillis;

    if (disabled == false){
        // direct time comparation only with rounded time! (millis*0.1)
        // uncomment to verify multiple triggers with same time value
        // if(roundedTime == startTime) __debugPrint(time, true);
        if(roundedTime == startTime && !pendingPlay){ 
            startPlay();
        }
        if(playbackPosition>=durationMillis && pendingPlay){
            endPlay();
        }
        // if(pendingPlay && usePressure){
        //     playPressure(roundedTime);
        // }
        if(pendingPlay){
            playbackPosition += diffTime;
            Serial.println(playbackPosition);
        }
    }

    if(pendingRecord){
        durationMillis += diffTime;
    }
    
    lastUpdateTimeMillis = currentTime; 
}


void NoteV2::createFromValues(uint8_t _index, uint8_t _padId, uint32_t _startTime,  uint16_t _durationMillis){
    reset();
    setDisabled(false);
    index = _index;
    padId = _padId;
    startTime = _startTime;
    durationMillis = _durationMillis;
}


uint8_t NoteV2::getOutput(){
    return output;
}


void NoteV2::startRecord(uint32_t roundedTime, uint8_t _index, uint8_t _padId){
    index = _index;
    padId = _padId;
    //if(usePressure) pNote->recordPosition = 0;

    durationMillis = 0;
    lastUpdateTimeMillis = millis();
    pendingRecord = true;
    startTime = roundedTime;
    startPlay();
}


void NoteV2::endRecord(uint32_t roundedTime){
    if(pendingRecord==false) return;

    pendingRecord = false;
    playbackPosition = durationMillis;

    setDisabled(false);
    endPlay();

}


void NoteV2::startPlay(){
    playbackPosition = 0;
    pendingPlay = true;
    if(usePressure==false) output = 255;
    //if(usePressure) pNote->playbackPosition = 0;
}


void NoteV2::endPlay(){
    pendingPlay = false;
    output = 0;
}
