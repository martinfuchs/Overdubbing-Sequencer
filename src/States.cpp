#include "States.h"


States::States() {
  setup();
}

void States::setup(){
}

void States::update(){

}

// MAIN STATE
int States::getMainState(){
    return currentMainState;
}

int States::getPrevMainState(){
    return prevMainState;
}

void States::mainStateChanged(int newState){
  Serial.println("State Changed");
}

void States::setNextMainState(){
  Serial.println("Next State");
  currentMainState++;
  if(currentMainState>=MAIN_END){
    currentMainState = 0;
  }
  mainStateChanged(currentMainState);
}

void States::setPrevMainState(){
  Serial.println("Prev State");
  currentMainState--;
  if(currentMainState<0){
    currentMainState = MAIN_END-1;
  }
  mainStateChanged(currentMainState);
}

void States::setMainState(int newState){
  prevMainState = prevMainState;
  currentMainState = newState;
  mainStateChanged(currentMainState);
}

// RECORDING
bool States::getRecording(){
  return recording;
}

void States::setRecording(bool value){
  recording = value;
}

// MENU1 SELECTION
int States::getMenu1State(){
    return currentMenu1Selection;
}

void States::setNextMenu1Selection(){
  if(currentMenu1Selection==MENU1_CHANNEL){
      currentSelectedSequence = 0;
  }
  if(currentMenu1Selection==MENU1_RECORD){
      currentSelectedSequence = MAX_SEQUENCES_SELECT-1;
  }

  if(currentMenu1Selection==MENU1_SEQUENCE){
    currentSelectedSequence++;

    if(currentSelectedSequence>=MAX_SEQUENCES_SELECT){
      currentMenu1Selection++;
    }
  }else{
    currentMenu1Selection++;
  }
  if(currentMenu1Selection>=MENU1_END){
    currentMenu1Selection = 0;
  }
}

void States::setPrevMenu1Selection(){
  if(currentMenu1Selection==MENU1_CHANNEL){
      currentSelectedSequence = 0;
  }
  if(currentMenu1Selection==MENU1_RECORD){
      currentSelectedSequence = MAX_SEQUENCES_SELECT-1;
  }

  if(currentMenu1Selection==MENU1_SEQUENCE){

    currentSelectedSequence--;

    if(currentSelectedSequence<0){
      currentMenu1Selection--;
    }
  }else{
    currentMenu1Selection--;
  }
  if(currentMenu1Selection<0){
    currentMenu1Selection = MENU1_END-1;
  }
}

// SEQUENCE SELECTION
int States::getCurrentSelectedSequence(){
  return currentSelectedSequence;
}


// CHANNEL SELECTION
int States::getSelectedChannel(){
    return selectedChannel;
}

void States::selectNextChannel(){
  selectedChannel++;
  if(selectedChannel>=CHANNEL_END){
    selectedChannel = 0;
  }
}

void States::selectPrevChannel(){
  selectedChannel--;
  if(selectedChannel<0){
    selectedChannel = CHANNEL_END-1;
  }
}


// VOUT SELECTION
int States::getSelectedVout(){
    return currentVout;
}

void States::selectNextVout(){
  currentVout++;
  if(currentVout>=VOUT_END){
    currentVout = 0;
  }
}

void States::selectPrevVout(){
  currentVout--;
  if(currentVout<0){
    currentVout = VOUT_END-1;
  }
}