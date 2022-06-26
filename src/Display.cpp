#include "Display.h"


Display::Display() {
  setup();
}


void Display::setup(){
  clear();
}


// WRITE FRAME
void Display::writeFrame(){
  setFrame(frame);
}


void Display::setFrame(int _frame[RES_Y][RES_X]){
  for (int y=0; y<7; y++){
      ledMatrix.writeRowToBytes(_frame[y],y);
      ledMatrix.writeBytesToSPI();
  }
}


// RENDER, CREATE FRAME
void Display::clear(){
  for (int y=0; y<7; y++){
        for (int x=0; x<10; x++){
            frame[y][x] = 0;
        }
    }
}


void Display::renderFillAll(){
  for (int y=0; y<7; y++){
        for (int x=0; x<10; x++){
            frame[y][x] = 1;
        }
    }
}


void Display::renderDebug(){
  int tmpframe[7][10] = {
     {0,0,0,0,0, 0,0,0,0,0},
     {0,1,1,1,1, 1,1,1,0,0},
     {0,1,0,0,0, 0,0,1,0,0},
     {0,1,0,0,0, 0,0,1,0,0},
     {0,1,1,1,1, 1,1,1,0,0},
     {0,0,0,0,0, 0,0,0,0,0},
     {0,0,0,0,0, 0,0,0,0,0}
  };
  setFrame(tmpframe);
}


void Display::renderChannelSelect(States* states){
  if(states->getSelectedVout()==States::VOUT_GATE1){
    addDotOnPosition(3,0);
  }else if(states->getSelectedVout()==States::VOUT_GATE2){
    addDotOnPosition(3,1);
  }else if(states->getSelectedVout()==States::VOUT_PRESSURE){
    addDotOnPosition(3,2);
  }
  if(states->getSelectedChannel()==States::CHANNEL_GATE1){
    addDotOnPosition(4,0);
  }else if(states->getSelectedChannel()==States::CHANNEL_GATE2){
    addDotOnPosition(4,1);
  }else if(states->getSelectedChannel()==States::CHANNEL_PRESSURE){
    addDotOnPosition(4,2);
  }
}


void Display::renderFrameSegment(int** _frame, int resx, int resy, int posx, int posy){
    for (int y=0; y<resy; y++){
        for (int x=0; x<resx; x++){
            frame[y+posy][x+posx] = _frame[y][x];
        }
    }
}


void Display::renderRandomState(bool enabled){
  if(enabled){
    addDotOnPosition(0,4);
    addDotOnPosition(1,4);
  }else{
    addDotOnPosition(0,4);
  }
}


void Display::renderState(States* states){
  int icon;
  if(states->getMainState()==States::MAIN_PLAY){
    if(states->getRecording()){
      icon = iconRecord;
    }else{
      icon = iconPlay;
    }
  }else if(states->getMainState()==States::MAIN_NOTEEDIT){
    icon = iconNoteEdit;
  }else if(states->getMainState()==States::MAIN_SAVE){
    icon = iconSave;
  }

  add3x3SectionOnPosition(0,0,icon);
}


void Display::renderMenu1Selection(States* states){
  int state = states->getMenu1State();
  if(state==States::MENU1_RECORD){
    int icon;
    if(states->getRecording()){
      icon = iconRecord;
    }else{
      icon = iconPlay;
    }
    add3x3SectionOnPosition(0,0,icon);
  }else if(state==States::MENU1_CHANNEL){
    if(states->getSelectedChannel()==States::CHANNEL_GATE1){
      addDotOnPosition(4,0);
    }else if(states->getSelectedChannel()==States::CHANNEL_GATE2){
      addDotOnPosition(4,1);
    }else if(states->getSelectedChannel()==States::CHANNEL_PRESSURE){
      addDotOnPosition(4,2);
    }
  }else if(state==States::MENU1_OUTPUT){
    if(states->getSelectedVout()==States::VOUT_GATE1){
      addDotOnPosition(3,0);
    }else if(states->getSelectedVout()==States::VOUT_GATE2){
      addDotOnPosition(3,1);
    }else if(states->getSelectedVout()==States::VOUT_PRESSURE){
      addDotOnPosition(3,2);
    }
  }else if(state==States::MENU1_SEQUENCE){
    int selectedSeq = states->getCurrentSelectedSequence();
    int yPos = 0;
    int xPos = 5+selectedSeq;
    if(selectedSeq>=5){
      yPos = 1;
      xPos = selectedSeq;
    }
    if(yPos>1) return;
    addDotOnPosition(xPos,yPos);
    // if(states->isEmptySelectedSequence()){
    //   addDotBlinkOnPosition(xPos,yPos);
    // }else{
    //   addDotOnPosition(xPos,yPos);
    // }
  }
}


/// HELPER

void Display::add3x3SectionOnPosition(int posx, int posy, int section[3][3]){
  for(int x=2; x>=0; x--){
		for(int y=0; y<3; y++){
      frame[posy+y][posx+x] = section[y][x];
		}
	}
}

void Display::addDotOnPosition(int posx, int posy){
  frame[posy][posx] = 1;
}

void Display::addDotBlinkOnPosition(int posx, int posy){
  blinkCounter += blinkSpeed;
  if(blinkCounter>100){
    addDotOnPosition(posx,posy);
    if(blinkCounter>150){
      blinkCounter = 0;
    }
  }
}


