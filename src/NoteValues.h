#ifndef NoteValues_h
#define NoteValues_h

#include <Arduino.h>
#include <Array.h>
#include "TouchPads.h"

#define MAX_NOTE_VAL 65535

#define MIN_ADD 100
#define MAX_ADD 6000

#define NUM_TOUCHPADS 12


class NoteValues
{
    public:
        NoteValues();
        void updateFromRotaryInput(int rotation);
        void updateFromTouchInput(Array<TouchPads::Touch*,NUM_TOUCHPADS> touches);
        int getPreviewNote();
        void toggleAddStep();
        void saveSelectedNote();
        int** getNoteDisplay();
        Array<int, 12> getNoteValues();
        void applyNotes(Array<int, 12> notes);
        uint8_t getLowest();


    private:
        void setup();
        void setupNoteDisplay();
        void addRotation(int value);
        void loadNoteValues();

        int rotaryNoteValue = MAX_NOTE_VAL;
        Array<int, 12> notes;

        bool fastStep = false;
        int addStep = MIN_ADD;
        int selectedNote = 0;

        int** noteFrame;

        float blinkCounter = 0;
        float blinkSpeed = 0.03;

        int defaultNoteValues[12] = {53000,50000,47000,45000,42000,41000,39000,35000,30000,29000,25000,23000};
        
};

#endif