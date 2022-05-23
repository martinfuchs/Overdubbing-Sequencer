#include <Array.h>
#include <PButton.h>
#include <TouchPads.h>
#include <Channel.h>
#include <Display.h>
#include <ClickEncoder.h>
#include <States.h>
#include <NoteValues.h>
#include <SaveHandler.h>
#include "DAC16.h"

// STATES
States states;
bool selectMainState = false;

// TOUCHPADS
TouchPads touchPads;
bool hasNewTouchInput = false;

const long eventTime_updateTouchPad = 4; //in ms
unsigned long previousTime_updateTouchPad = 0;

// SEQUENCER
Channel channel1;
Channel channel2;
Channel channel3;

const long eventTime_updateSequencer = 2; //in ms
unsigned long previousTime_updateSequencer  = 0;

const long eventTime_updateDAC = 4; //in ms
unsigned long previousTime_updateDAC  = 0;

int** sequencerFrame;

// DAC
DAC16 dac16;
int activeNoteValue = -1;

// LEDS
uint8_t Led1_Pin = 6;
uint8_t Led2_Pin = 8;

// BUTTONS
PButton button1(5);
PButton button2(7);

// CLOCK/RESET
uint8_t Clock_Pin = 21;
uint8_t Reset_Pin = 20;
float interpolatedClockVal = 0.0;
float interpolatedResetVal = 0.0;
int prevClockVal = -1;
int prevResetVal = -1;

// ROTARY ENCODER
const long eventTime_updateRotEnc = 1; //in ms
unsigned long previousTime_updateRotEnc  = 0;

constexpr uint8_t PIN_ENCA = 0;
constexpr uint8_t PIN_ENCB = 1;
constexpr uint8_t PIN_BTN = 2;
constexpr uint8_t ENC_STEPSPERNOTCH = 4;
constexpr uint8_t PRINT_BASE = 10;
// constexpr uint16_t ENC_SERVICE_US = 1000; // 1ms
constexpr bool BTN_ACTIVESTATE = LOW;
static ClickEncoder clickEncoder{PIN_ENCA, PIN_ENCB, PIN_BTN, ENC_STEPSPERNOTCH, BTN_ACTIVESTATE};

// DISPLAY
Display display;

const long eventTime_updateDisplayLayer1 = 6; //in ms
const long eventTime_updateDisplayLayer2 = 2; //in ms

unsigned long previousTime_updateDisplayLayer1 = 0;
unsigned long previousTime_updateDisplayLayer2 = 0;


// NOTEVALUES
NoteValues noteValues;

// EEPROM, SAVEHANDLER
SaveHandler saveHandler;



///////////////////////////////////
//  SAVE HANDLER
///////////////////////////////////

void saveAll(){
  int addressIndex = 0;
  addressIndex = saveHandler.saveSequences(addressIndex, channel1.getSequences(), channel1.getNumSequences(), channel1.getNumNotesPerSequence());
  addressIndex = saveHandler.saveSequences(addressIndex, channel2.getSequences(), channel2.getNumSequences(), channel2.getNumNotesPerSequence());
  //addressIndex = saveHandler.saveSequences(addressIndex, channel3.getSequences(), channel3.getNumSequences(), channel3.getNumNotesPerSequence());
  addressIndex = saveHandler.saveNotes(addressIndex, noteValues.getNoteValues());
  Serial.print("Saved bytes: ");
  Serial.print(addressIndex);
  Serial.print("/");
  Serial.println(2048); //Teensy 3.1	2048 bytes EEPROM
  Serial.print("Bytes remaining: ");
  Serial.println(2048-addressIndex);
}

void loadAll(){
  int addressIndex = 0;
  addressIndex = saveHandler.loadSequences(addressIndex, channel1.getSequences(), channel1.getNumSequences(), channel1.getNumNotesPerSequence());
  addressIndex = saveHandler.loadSequences(addressIndex, channel2.getSequences(), channel2.getNumSequences(), channel2.getNumNotesPerSequence());
  //addressIndex = saveHandler.loadSequences(addressIndex, channel3.getSequences(), channel3.getNumSequences(), channel3.getNumNotesPerSequence());
  noteValues.applyNotes(saveHandler.loadNotes(addressIndex));
}


///////////////////////////////////
// MAIN SETUP
///////////////////////////////////

void setup(){
  Serial.begin(115200);
  Serial.println("start");

  // DAC/DISPLAY
  SPI.begin();

  // DAC
  // init with maximum voltage -> lowest tone
  activeNoteValue = 0;
  dac16.write(0);
  delay(100);
  dac16.write(65535);

  // TOUCHPADS
  touchPads.setup();

  // LEDS
  pinMode(Led1_Pin, OUTPUT);
  pinMode(Led2_Pin, OUTPUT);

  // BUTTONS
  button1.begin();
  button2.begin();

  // CLOCK/RESET
  pinMode(Clock_Pin, INPUT_PULLUP);
  pinMode(Reset_Pin, INPUT_PULLUP);

  // SEQUENCER
  //int _type, uint8_t _numSequences, uint8_t _numNotesPerSequences, bool _usePressure, bool _debug
  channel1.setup(0, 10, 20, false, false);
  channel2.setup(1, 10, 20, false, false);
  channel3.setup(2, 5, 5, true, false);

  //ROTARY ENCODER
  //clickEncoder.setAccelerationEnabled(true);
  clickEncoder.setDoubleClickEnabled(true);
  clickEncoder.setLongPressRepeatEnabled(true);

  // APPLY DEFAULT NOTEVALUES
  channel1.setNoteValueRef(&noteValues);
  channel2.setNoteValueRef(&noteValues);
  channel3.setNoteValueRef(&noteValues);

  // ADD FIRST SEQUENCE
  channel1.enableSequence(0);
  channel2.enableSequence(0);
  channel3.enableSequence(0);

  // LOAD DEFAULT
  loadAll();
}

///////////////////////////////////
//  HELPER
///////////////////////////////////

Channel* getSelectedChannel(){
  if(states.getSelectedChannel()==States::CHANNEL_GATE1){
    return &channel1;
  }else if(states.getSelectedChannel()==States::CHANNEL_GATE2){
    return &channel2;
  }else if(states.getSelectedChannel()==States::CHANNEL_PRESSURE){
    return &channel3;
  }
}

Channel* getSelectedVoutChannel(){
  if(states.getSelectedVout()==States::VOUT_GATE1){
    return &channel1;
  }else if(states.getSelectedVout()==States::VOUT_GATE2){
    return &channel2;
  }else if(states.getSelectedVout()==States::VOUT_PRESSURE){
    return &channel3;
  }
}


///////////////////////////////////
// ROTARY ENCODER
///////////////////////////////////

// depending on framerate? sometimes not triggered. do we need an other update handling? or just faster hardware :)

void updateClickEncoderButtonState()
{
    switch (clickEncoder.getButton())
    {
    case Button::Clicked:
        selectMainState = false;
        Serial.println("Button clicked");
        break;
    case Button::DoubleClicked:
        Serial.println("Button doubleClicked");
        break;
    case Button::Held:
        selectMainState = true;
        Serial.println("Button Held");
        break;
    case Button::LongPressRepeat:
        Serial.println("Button longPressRepeat");
        break;
    case Button::Released:
        selectMainState = false;
        Serial.println("Button released");
        break;
    default:
        break;
    }
}

void updateClickEncoderValue()
{ 
    int16_t value = clickEncoder.getIncrement();

    if(selectMainState){
        //Serial.println(value);
        if (value == -1) states.setNextMainState();
        if (value == 1) states.setPrevMainState();

    }else if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
        if (value == -1) states.setNextMenu1Selection();
        if (value == 1) states.setPrevMenu1Selection();

    }else if(states.getMainState()==States::MAIN_NOTEEDIT){ //-------------------------> MAIN_NOTEEDIT
        noteValues.updateFromRotaryInput(value);
    }

    // if (value != 0)
    // {
    //     Serial.print("Encoder value: ");
    //     Serial.print(value, PRINT_BASE);
    //     Serial.print(" ");
    // }
}

///////////////////////////////////
//  DISPLAY
///////////////////////////////////
// Render two layers. Second layer with more update ratio and therefor brighter

void updateDisplayLayer1(){
    display.clear();

    if(selectMainState){
      //display.renderStateSelect(&states);
      display.renderFillAll();
  
    }else if(states.getMainState() == States::MAIN_PLAY){ //-------------------------> MAIN_PLAY 
      display.renderChannelSelect(&states);
      display.renderFrameSegment(getSelectedChannel()->getFrameDisplay(),5,4,5,3);
      display.renderFrameSegment(getSelectedChannel()->getSequencesDisplay(),5,2,5,0);

    }else if(states.getMainState() == States::MAIN_NOTEEDIT){ //-------------------------> MAIN_NOTEEDIT
      display.renderFrameSegment(noteValues.getNoteDisplay(),5,7,5,0);
    }

    display.renderState(&states);

    display.writeFrame();
}

void updateDisplayLayer2(){
    display.clear();

    if(selectMainState){
      display.renderState(&states);

    }else if(states.getMainState() == States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
      display.renderMenu1Selection(&states);
      if(states.getMenu1State() == States::MENU1_SEQUENCE){
        display.renderFrameSegment(getSelectedChannel()->getFrameDisplay(),5,4,5,3);
      }
    }

    display.writeFrame();
}


///////////////////////////////////
//  TOUCHPADS -> SEQUENCER / TOUCHPADS -> NOTEVALUES
///////////////////////////////////

void updateTouchpads(){
  touchPads.update();

    hasNewTouchInput = touchPads.hasNewInput();
    if(hasNewTouchInput){
      if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
        getSelectedChannel()->updateFromTouchInput(touchPads.getTouches(),states.getRecording());

      }else if(states.getMainState()==States::MAIN_NOTEEDIT){//-------------------------> MAIN_NOTEEDIT
        noteValues.updateFromTouchInput(touchPads.getTouches());
      }
    }
}

///////////////////////////////////
//  SEQUENCER
///////////////////////////////////

void updateSequencer(){
  if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
      channel1.update();
      channel2.update();
      channel3.update();

      int newNoteValue = getSelectedVoutChannel()->getActiveNoteValue();
      if(newNoteValue != 0){
        activeNoteValue = newNoteValue;
      }
  }
}


///////////////////////////////////
//  DAC
///////////////////////////////////

void updateDAC(){
  if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
    if(activeNoteValue > 0){
      dac16.write(activeNoteValue);
    }
  }
  if(states.getMainState()==States::MAIN_NOTEEDIT){ //-------------------------> MAIN_NOTEEDIT
      dac16.write(noteValues.getPreviewNote());
  }
}


///////////////////////////////////
// BUTTONS
///////////////////////////////////

void button1Released(){
  if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
    if(states.getMenu1State()==States::MENU1_RECORD){
      states.setRecording(!states.getRecording());
    }else if(states.getMenu1State()==States::MENU1_CHANNEL){
      states.selectNextChannel();
      states.setMaxSequences(getSelectedChannel()->getNumSequences());
    }else if(states.getMenu1State()==States::MENU1_OUTPUT){
      states.selectNextVout();
    }else if(states.getMenu1State()==States::MENU1_SEQUENCE){
      if(getSelectedChannel()->isSequenceEnabled(states.getCurrentSelectedSequence())==false){
        getSelectedChannel()->enableSequence(states.getCurrentSelectedSequence());
      }else{
        getSelectedChannel()->triggerSequence(states.getCurrentSelectedSequence());
      }
    }
  }else if(states.getMainState()==States::MAIN_NOTEEDIT){ //-------------------------> MAIN_NOTEEDIT
    noteValues.toggleAddStep();
  }else if(states.getMainState()==States::MAIN_SAVE){ //-------------------------> MAIN_SAVE
    loadAll();
    //noteValues.applyNotes(saveHandler.loadNotes());
  }
}

void button2Released(){
  int timeChangedMillis = button2.get_time_changed();
  if(timeChangedMillis>1000){
    // LONG PRESSED
    if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
      if(states.getMenu1State()==States::MENU1_SEQUENCE){
        if(getSelectedChannel()->isSequenceEnabled(states.getCurrentSelectedSequence())){
          getSelectedChannel()->disableSequence(states.getCurrentSelectedSequence());
        }
      }
    }
  }else{
    // SHORT PRESSED
    if(states.getMainState()==States::MAIN_PLAY){ //-------------------------> MAIN_PLAY
      if(states.getMenu1State()==States::MENU1_SEQUENCE){
        getSelectedChannel()->undo();
      }
    }else if(states.getMainState()==States::MAIN_NOTEEDIT){ //-------------------------> MAIN_NOTEEDIT
      noteValues.saveSelectedNote();
    }else if(states.getMainState()==States::MAIN_SAVE){ //-------------------------> MAIN_SAVE
      saveAll();
      //saveHandler.saveNotes(noteValues.getNoteValues());
    }
  }
}



///////////////////////////////////
// MAIN LOOP
///////////////////////////////////

uint32_t FreeMem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}


void loop(){
  int currentTime = millis();

  //ROTARY ENCODER 
  if ( currentTime - previousTime_updateRotEnc >= eventTime_updateRotEnc) {
    clickEncoder.service();
    previousTime_updateRotEnc = currentTime;
  }
  updateClickEncoderButtonState();
  updateClickEncoderValue();

  // DAC
  if ( currentTime - previousTime_updateDAC >= eventTime_updateDAC) {
    updateDAC();
    previousTime_updateDAC = currentTime;
  }

  // DISPLAY
  if ( currentTime - previousTime_updateDisplayLayer1 >= eventTime_updateDisplayLayer1) {
    updateDisplayLayer1();
    previousTime_updateDisplayLayer1 = currentTime;
  }
  if ( currentTime - previousTime_updateDisplayLayer2 >= eventTime_updateDisplayLayer2) {
    updateDisplayLayer2();
    previousTime_updateDisplayLayer2 = currentTime;
  }

  // TOUCHPADS -> SEQUENCER / TOUCHPADS -> NOTEVALUES 
  if ( currentTime - previousTime_updateTouchPad >= eventTime_updateTouchPad) {
    updateTouchpads();
    previousTime_updateTouchPad = currentTime;
  }

  // SEQUENCER 
  if ( currentTime - previousTime_updateSequencer >= eventTime_updateSequencer) {
    updateSequencer();
    previousTime_updateSequencer = currentTime;
  }


  // BUTTONS 
  if (button1.toggled()) {
    Serial.println(FreeMem());
      if (button1.read() == PButton::RELEASED){
        button1Released();
      }
  }

  if (button2.toggled()) {
      if (button2.read() == PButton::RELEASED){
        button2Released();
      }
  }

  // CLOCK/RESET 
  int clockVal = digitalRead(Clock_Pin);
  if(clockVal != prevClockVal){
    prevClockVal = clockVal;
    if(clockVal==LOW){
      channel1.triggerClock();
      channel2.triggerClock();
      channel3.triggerClock();
    }
  }

  int resetVal = digitalRead(Reset_Pin);
  if(resetVal != prevResetVal){
    prevResetVal = resetVal;
    if(resetVal==LOW){
      channel1.resetClock();
      channel2.resetClock();
      channel3.resetClock();
    }
  }

  // BUTTON LEDS 
  // interpolate for visibility
  interpolatedResetVal += (float(resetVal)-interpolatedResetVal)*0.01;
  interpolatedClockVal += (float(clockVal)-interpolatedClockVal)*0.01;
  if(states.getMainState()==States::MAIN_PLAY){
    if(interpolatedClockVal<0.99){
      digitalWrite(Led2_Pin, LOW);
    }else{
      digitalWrite(Led2_Pin, HIGH);
    }

    if(interpolatedResetVal<0.99){
      digitalWrite(Led1_Pin, HIGH);
    }else{
      digitalWrite(Led1_Pin, LOW);
    }
  }else{ // right led always on, left off
      digitalWrite(Led2_Pin, HIGH);
      digitalWrite(Led1_Pin, LOW);
  }


}
