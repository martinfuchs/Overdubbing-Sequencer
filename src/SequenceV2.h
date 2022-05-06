#ifndef SequenceV2_h
#define SequenceV2_h

#include <Array.h>
#include "TouchPads.h"
#include "NoteValues.h"
#include "NoteV2.h"

#define MAX_NOTES 20 //!>uint8_t
#define NUM_TOUCHPADS 12


class SequenceV2
{
  public:
    SequenceV2();

    void setup(bool printDebug=false);
    void update();
    int getOutputValue();

    void setTouchInputs(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches, bool record);
    void stopPendingInputs();

    void play();
    void stop();
    void restart();

    void setEnabled(bool value);
    bool getEnabled();

    void clearAll();
    void undo();

    void setNoteValueRef(NoteValues *notevalues);
    int getActiveNoteValue(); // NOTE VALUE FOR DAC

    int** getFrameDisplay();


  private:
    void setup();
    void startNoteRecord(TouchPads::Touch* touch);
    void endNoteRecord(TouchPads::Touch* touch);
    void updateNoteRecord(TouchPads::Touch* touch);

    void startLiveOutput(TouchPads::Touch* touch);
    void updateLiveOutput(TouchPads::Touch* touch);
    void endLiveOutput(TouchPads::Touch* touch);

    void setupFrameDisplay();
    void setupSequenceNotes();

    int getNoteValue(int padId);

    void __debugPrint(String message);

    Array<NoteV2*,MAX_NOTES> sequence;
    Array<NoteV2*,NUM_TOUCHPADS> pendingInputNotes;
    int currentNoteIndex = 0;

    unsigned long startTime = 0;
    int roundedTime = 0;

    bool printDebug = false;

    bool enabled = false;

    // OUTPUT
    //uint8_t outputPin = -1;
    //bool usePressureValues = false;
    int liveOutputValue = 0.0;
    int sequenceOutputValue = 0.0;

    // DISPLAY
    int** frame;

    //NOTEVALUES
    NoteValues *notevalues;
    bool hasNoteValues = false;
    int liveOutputNoteValue = -1;
    int sequenceOutputNoteValue = -1;

    // PLAYBACK
    boolean playing = false;
    boolean recording = false;
    boolean handsOn = false;

};
#endif