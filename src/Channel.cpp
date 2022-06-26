
#include "Channel.h"

Channel::Channel(){ 
}

void Channel::setup(int _type, uint8_t _numSequences, uint8_t _numNotesPerSequences, bool _usePressure, bool _debug){
    type = _type;
    numSequences = _numSequences;
    usePressure = _usePressure;
    numNotesPerSequences = _numNotesPerSequences;
    debug = _debug;

    setupEmptySequences();
    setupSequencesDisplay();

    if(type==TYPE_GATE1){
        pinMode(Gate1_Pin, OUTPUT);
    }   
    if(type==TYPE_GATE2){
        pinMode(Gate2_Pin, OUTPUT);
    }
    if(type==TYPE_ENVELOPE){
        pinMode(Pressure_Pin, OUTPUT);
        //analogWriteResolution(12);
    }
}


SequenceV2* Channel::getActivePlayingSequence(){
    return sequences[activePlayingSequenceIndex];
}

SequenceV2* Channel::getActiveRecordingSequence(){
    return sequences[activeRecordingSequenceIndex];
}


void Channel::setupEmptySequences(){
    for(uint8_t i=0; i<numSequences; i++){
        SequenceV2* newSequence = new SequenceV2();
        newSequence->setup(numNotesPerSequences, usePressure, debug);
        newSequence->setEnabled(false);
        //sequences[i] = newSequence;
        sequences.add(newSequence);
    }
}

void Channel::playSequence(int index){
    SequenceV2* sequence = sequences[index];
    sequence->play();
}

bool Channel::isSequencePlaying(int index){
    SequenceV2* sequence = sequences[index];
    return sequence->getPlaying();
}

void Channel::setRecordingSequence(int index){
    activeRecordingSequenceIndex = index;
}

void Channel::setAutoRecording(bool value){
    autoRecording = value;
}

void Channel::enableSequence(int index, bool play){
    SequenceV2* sequence = sequences[index];
    sequence->setEnabled(true);
    if(play) sequence->play();
}

void Channel::disableSequence(int index){
    if(index<=0){
        sequences[0]->clearAll();
        return;
    }

    SequenceV2* sequence = sequences[index];
    sequence->setEnabled(false);
    sequence->clearAll();
}

bool Channel::isSequenceEnabled(int index){
    if(index>=numSequences) return false;
    return sequences[index]->getEnabled();
}


void Channel::update(){

    if(autoRecording){
        activeRecordingSequenceIndex = activePlayingSequenceIndex;
    }

    getActivePlayingSequence()->update();
    if(activePlayingSequenceIndex!=activeRecordingSequenceIndex){
        getActiveRecordingSequence()->update();
    }

    uint8_t output = getActivePlayingSequence()->getOutputValue();
    uint8_t second_output = 0;
    if(activePlayingSequenceIndex!=activeRecordingSequenceIndex){
        second_output = getActiveRecordingSequence()->getOutputValue();
    }
    if(second_output>output){
        output = second_output;
    }

    if(type==TYPE_GATE1){
        if(output==255){
            digitalWrite(Gate1_Pin, HIGH);
        }else{
            digitalWrite(Gate1_Pin, LOW);
        }
    }   
    if(type==TYPE_GATE2){
        if(output==255){
            digitalWrite(Gate2_Pin, HIGH);
        }else{
            digitalWrite(Gate2_Pin, LOW);
        }
    }
    if(type==TYPE_ENVELOPE){
        analogWrite(Pressure_Pin, output);
    }
    
}


void Channel::setNoteValueRef(NoteValues *notevalues){
    for(uint8_t i=0; i<numSequences; i++){
        sequences[i]->setNoteValueRef(notevalues);
    }
}

void Channel::updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record){
    getActiveRecordingSequence()->setTouchInputs(touches,record);
}


void Channel::clearAll(){
    getActivePlayingSequence()->clearAll();
}


void Channel::undo(int sequenceIndex){
    // if(sequenceIndex<=0 || sequenceIndex>numSequences-1){
    //     return;
    // }

    SequenceV2* sequence = sequences[sequenceIndex];
    sequence->undo();
    //getActiveRecordingSequence()->undo();
}


void Channel::resetClock(){
    getActivePlayingSequence()->restart();
    if(activePlayingSequenceIndex!=activeRecordingSequenceIndex){
        getActiveRecordingSequence()->restart();
    }
}


void Channel::triggerSequence(int sequenceIndex){
    //getActivePlayingSequence()->stopPendingInputs();
    activePlayingSequenceIndex = sequenceIndex;
    //getActivePlayingSequence()->restart();
}


void Channel::triggerClock(){
    getActiveRecordingSequence()->stopPendingInputs();
    nextSequence();

    getActivePlayingSequence()->restart();
    if(activePlayingSequenceIndex!=activeRecordingSequenceIndex){
        getActiveRecordingSequence()->restart();
    }
}


void Channel::nextSequenceIndex(){
    if(randomMode){
        activePlayingSequenceIndex+=random(0,4);
    }else{
        activePlayingSequenceIndex++;
    }
    if(activePlayingSequenceIndex>numSequences-1){
        activePlayingSequenceIndex = 0;
    }
}


void Channel::nextSequence(){
    // -> todo: use disabled sequences as gap
    nextSequenceIndex();
    while(getActivePlayingSequence()->getEnabled()==false || getActivePlayingSequence()->getPlaying()==false){
        nextSequenceIndex();
    }
}

// void Channel::prevSequence(){
//     activePlayingSequenceIndex--;
//     if(activePlayingSequenceIndex<0){
//         activePlayingSequenceIndex = numSequences-1;
//     }
// }


int** Channel::getFrameDisplay(){
    return getActivePlayingSequence()->getFrameDisplay();
}


int Channel::getActiveNoteValue(){

    uint8_t output = getActivePlayingSequence()->getOutputValue();
    uint8_t second_output = 0;
    if(activePlayingSequenceIndex!=activeRecordingSequenceIndex){
        second_output = getActiveRecordingSequence()->getOutputValue();
    }

    int noteValue = getActivePlayingSequence()->getActiveNoteValue();
    if(second_output!=0 && second_output>=output){
        noteValue = getActiveRecordingSequence()->getActiveNoteValue();
    }
    return noteValue;
}


uint8_t Channel::getNumSequences(){
    return numSequences;
}

uint8_t Channel::getNumNotesPerSequence(){
    return numNotesPerSequences;
}

ustd::array<SequenceV2*> Channel::getSequences(){
    return sequences;
}


bool Channel::getRandomMode(){
    return randomMode;
}

void Channel::setRandomMode(bool value){
    randomMode = value;
}


//////////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////////

void Channel::setupSequencesDisplay(){
    frame = new int*[2];
    for (int i = 0; i < 2; i++){
        frame[i] = new int[5];
    }
}

int** Channel::getSequencesDisplay(){
    int sequenceIndex = 0;
    for(int y=0; y<2; y++){
        for(int x=0; x<5; x++){
            frame[y][x] = 0;
            if(sequenceIndex<numSequences){

                if(sequences[sequenceIndex]->getEnabled()){
                    frame[y][x] = 1;

                    if(sequenceIndex == activePlayingSequenceIndex){
                        blinkCounter += blinkSpeed;
                        if(blinkCounter>20){
                            frame[y][x] = 0;
                            if(blinkCounter>25){
                            blinkCounter = 0;
                            }
                        }else{
                            frame[y][x] = 1;
                        }
                    }
                }else{
                    frame[y][x] = 0;
                }
            }
            sequenceIndex++;
        }
    }
    return frame;
}
