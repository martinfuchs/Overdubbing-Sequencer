#ifndef SequenceV2_h
#define SequenceV2_h

#include <Array.h>
#include "TouchPads.h"
#include "NoteValues.h"
#include "NoteV2.h"
#include <ustd_array.h>
#include <ustd_map.h>
#include "PressureNote.h"


#define MAX_NOTES_DISPLAY 20
#define NUM_TOUCHPADS 12

class SequenceV2
{
  public:
    SequenceV2();

    void setup(uint8_t numNotes, bool _usePressureValues, bool printDebug);
    void update();
    uint8_t getOutputValue(); // 0-255

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
    uint16_t getActiveNoteValue(); // NOTE VALUE FOR DAC

    int** getFrameDisplay();

    ustd::array<NoteV2*> getNoteArray();
    uint8_t getNumNotes();

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

    uint16_t getNoteValue(uint8_t padId);

    void __debugPrint(String message);

    bool printDebug = false;

    // NOTES
    uint8_t numNotes = 0;
    ustd::array<NoteV2*> sequence;
    //Array<NoteV2*,MAX_NOTES> sequence;
    Array<NoteV2*,NUM_TOUCHPADS> pendingInputNotes;
    int currentNoteIndex = 0;

    // TIME
    unsigned long startTime = 0;
    int roundedTime = 0;

    // OUTPUT
    bool enabled = false;
    bool usePressureValues = false;
    uint8_t liveOutputValue = 0;
    uint8_t sequenceOutputValue = 0;
    float interpolatedOutputValue = 0;

    // DISPLAY
    int** frame;

    //NOTEVALUES
    NoteValues *notevalues;
    bool hasNoteValues = false;
    uint16_t liveOutputNoteValue = 0;
    uint16_t sequenceOutputNoteValue = 0;

    // PLAYBACK
    boolean playing = false;
    boolean recording = false;
    boolean handsOn = false;

    // PRESSURE
    ustd::map<uint8_t, PressureNote*> pressureNotes;

};
#endif