
#include "Channel.h"

Channel::Channel(){ 
}

void Channel::setup(int _type, uint8_t _numSequences){
    type = _type;
    numSequences = _numSequences;

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
        analogWriteResolution(12);
    }
}


SequenceV2* Channel::getActiveSequence(){
    return sequences[activeSequenceIndex];
}


void Channel::setupEmptySequences(){
    for(uint8_t i=0; i<numSequences; i++){
        SequenceV2* newSequence = new SequenceV2();
        newSequence->setup(false);
        newSequence->setEnabled(false);
        //sequences[i] = newSequence;
        sequences.add(newSequence);
    }
}

void Channel::enableSequence(int index){
    SequenceV2* sequence = sequences[index];
    sequence->setEnabled(true);
    sequence->play();
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
    getActiveSequence()->update();

    int out = getActiveSequence()->getOutputValue();
    if(type==TYPE_GATE1){
        if(out==1){
            digitalWrite(Gate1_Pin, HIGH);
        }else{
            digitalWrite(Gate1_Pin, LOW);
        }
    }   
    if(type==TYPE_GATE2){
        if(out==1){
            digitalWrite(Gate2_Pin, HIGH);
        }else{
            digitalWrite(Gate2_Pin, LOW);
        }
    }
    if(type==TYPE_ENVELOPE){
        analogWrite(Pressure_Pin, out);
    }
    
}


void Channel::setNoteValueRef(NoteValues *notevalues){
    for(uint8_t i=0; i<numSequences; i++){
        sequences[i]->setNoteValueRef(notevalues);
    }
}

void Channel::updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record){
    getActiveSequence()->setTouchInputs(touches,record);
}


void Channel::clearAll(){
    getActiveSequence()->clearAll();
}


void Channel::undo(){
    getActiveSequence()->undo();
}


void Channel::resetClock(){
    getActiveSequence()->restart();
}


void Channel::triggerSequence(int sequenceIndex){
    //getActiveSequence()->stopPendingInputs();
    activeSequenceIndex = sequenceIndex;
    //getActiveSequence()->restart();
}


void Channel::triggerClock(){
    getActiveSequence()->stopPendingInputs();
    nextSequence();
    getActiveSequence()->restart();
}


void Channel::nextSequenceIndex(){
    activeSequenceIndex++;
    if(activeSequenceIndex>numSequences-1){
        activeSequenceIndex = 0;
    }
}


void Channel::nextSequence(){
    // -> todo: use disabled sequences as gap
    nextSequenceIndex();
    while(getActiveSequence()->getEnabled()==false){
        nextSequenceIndex();
    }
}

// void Channel::prevSequence(){
//     activeSequenceIndex--;
//     if(activeSequenceIndex<0){
//         activeSequenceIndex = numSequences-1;
//     }
// }


int** Channel::getFrameDisplay(){
    return getActiveSequence()->getFrameDisplay();
}


int Channel::getActiveNoteValue(){
    return getActiveSequence()->getActiveNoteValue();
}


uint8_t Channel::getNumSequences(){
    return numSequences;
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
            if(sequenceIndex<numSequences){

                if(sequences[sequenceIndex]->getEnabled()){
                    frame[y][x] = 1;

                    if(sequenceIndex == activeSequenceIndex){
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
