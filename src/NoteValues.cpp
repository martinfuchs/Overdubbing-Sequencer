#include "NoteValues.h"


NoteValues::NoteValues() {
  setup();
}

void NoteValues::setup(){
    loadNoteValues();
    setupNoteDisplay();
}


void NoteValues::loadNoteValues(){
    for(int i=0; i<NUM_TOUCHPADS; i++){
        notes[i] = defaultNoteValues[i]; 
    }
}


void NoteValues::toggleAddStep(){
    fastStep = !fastStep;

    if(fastStep){
        addStep = MAX_ADD;
    }else{
        addStep = MIN_ADD;
    }
}

void NoteValues::applyNotes(Array<int, 12> newNotes){
    for(int i=0; i<12; i++){
        notes[i] = newNotes[i]; 
    }
}

Array<int, 12> NoteValues::getNoteValues(){
    return notes;
}


void NoteValues::updateFromRotaryInput(int rotation){
    addRotation(rotation);
}


void NoteValues::addRotation(int value){
    rotaryNoteValue += addStep*value;
    rotaryNoteValue = constrain(rotaryNoteValue, 0, MAX_NOTE_VAL);
    //Serial.println(rotaryNoteValue);
}

void NoteValues::saveSelectedNote(){
    notes[selectedNote] = rotaryNoteValue;
}


void NoteValues::updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches){
    for (uint8_t i=0; i<12; i++) {
        if(touches[i]->eventType == TouchPads::TOUCH_PRESSED){
            //Serial.println("pressed");
            selectedNote = i;
            rotaryNoteValue = notes[selectedNote];
        }
        if(touches[i]->eventType == TouchPads::TOUCH_RELEASED){
            //Serial.println("released");
            //
        }
        if(touches[i]->eventType == TouchPads::TOUCH_PENDING){
            //Serial.println("pending");
            //
        }
    }
}

int NoteValues::getPreviewNote(){
    return rotaryNoteValue;
}


// DISPLAY
void NoteValues::setupNoteDisplay(){
    noteFrame = new int*[7];
    for (int i = 0; i < 7; i++){
        noteFrame[i] = new int[5];
    }
}


int** NoteValues::getNoteDisplay(){
    for (int y=0; y<7; y++){
        for (int x=0; x<5; x++){
            noteFrame[y][x] = 0;
        }
    }

    int x = 0;
    int y = 0;
    int mappedNoteValue = map(rotaryNoteValue,0,MAX_NOTE_VAL,0,35); 

    for(int i=0; i<35; i++){
        if(i<mappedNoteValue){ 
            noteFrame[y][x] = 0;
        }else{
            noteFrame[y][x] = 1;
        }
        x++;
        if(x>4){
            x=0;
            y++;
        }
    }

    return noteFrame;
}