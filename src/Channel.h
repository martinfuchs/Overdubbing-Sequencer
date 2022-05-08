
#ifndef CHANNEL_H
#define CHANNEL_H

#include "Arduino.h"
#include "NoteValues.h"
#include "SequenceV2.h"
#include <ustd_array.h>

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
  void setup(int _type, uint8_t _numSequences, uint8_t _numNotesPerSequences, bool _usePressure, bool _debug);
  void update();
  void setNoteValueRef(NoteValues *notevalues);
  void updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record);
  void clearAll();
  void undo();
  void triggerClock();
  void resetClock();
  int** getFrameDisplay();
  int getActiveNoteValue(); // NOTE VALUE FOR DAC
  uint8_t getNumSequences();
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
    uint8_t type = TYPE_NONE;

    uint8_t activeSequenceIndex = 0;

    uint8_t numSequences;
    ustd::array<SequenceV2*> sequences;

    uint8_t numNotesPerSequences = 20;

    bool usePressure = false;
    bool debug = false;


    // GATES
    uint8_t Gate1_Pin = 15;
    uint8_t Gate2_Pin = 14;

    // PRESSURE
    uint8_t Pressure_Pin = A14;

    // DISPLAY
    int** frame;

    //
    uint8_t blinkCounter = 0;
    uint8_t blinkSpeed = 1;

};

#endif