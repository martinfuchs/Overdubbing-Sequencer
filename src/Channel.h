
#ifndef CHANNEL_H
#define CHANNEL_H

#include "Arduino.h"
#include "NoteValues.h"
#include "SequenceV2.h"
#include <Array.h>

#define MAX_SEQUENCES 10
#define NUM_TOUCHPADS 12


class Channel
{ 
public:
    enum CHANNEL_TYPE{
        TYPE_GATE1,
        TYPE_GATE2,
        TYPE_ENVELOPE,
        TYPE_NONE,
        TYPE_END
    };

  Channel();
  void setup(int type);
  void update();
  void setNoteValueRef(NoteValues *notevalues);
  void updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record);
  void clearAll();
  void undo();
  void triggerClock();
  void resetClock();
  int** getFrameDisplay();
  int getActiveNoteValue(); // NOTE VALUE FOR DAC
  int getNumSequences();
  void triggerSequence(int sequenceIndex);

  //
  void enableSequence(int index);
  void disableSequence(int index);
  bool isSequenceEnabled(int index);

  int** getSequencesDisplay();

  
private:
    // TIMER
    void nextSequence();
    //void prevSequence();
    void nextSequenceIndex();

    // SEQUENCES
    void setupEmptySequences();
    void setupSequencesDisplay();
    SequenceV2* getActiveSequence();
    int type = TYPE_NONE;

    uint8_t activeSequenceIndex = 0;

    Array<SequenceV2*,MAX_SEQUENCES> sequences;

    // GATES
    uint8_t Gate1_Pin = 15;
    uint8_t Gate2_Pin = 14;

    // PRESSURE
    uint8_t Pressure_Pin = A14;

    // DISPLAY
    int** frame;

    // NOTEVALUES
    NoteValues* notevalues;

    //
    int blinkCounter = 0;
    int blinkSpeed = 1;

};

#endif