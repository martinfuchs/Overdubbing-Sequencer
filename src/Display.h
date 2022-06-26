#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <LedMatrix.h>
#include <States.h>


class Display
{
    public:
        Display();
        void renderDebug();
        void renderFillAll();
        void clear();


        void renderState(States* states);
        void renderChannelSelect(States* states);
        void renderMenu1Selection(States* states);
        void renderFrameSegment(int** _frame, int resx, int resy, int posx, int posy);
        void renderRandomState(bool enabled);

        void writeFrame();


    
    private:
        void setup();
        void setFrame(int _frame[RES_Y][RES_X]);
        void add3x3SectionOnPosition(int posx, int posy, int section[3][3]);
        void addDotOnPosition(int posx, int posy);
        void addDotBlinkOnPosition(int posx, int posy);


        LedMatrix ledMatrix;
        int frame[7][10];

        int blinkCounter = 0;
        int blinkSpeed = 1;


        int iconRecord[5][3] = {
            {1,1,1},
            {1,1,1},
            {1,1,1}
            };
        
        int iconPlay[5][3] = {
            {1,1,1},
            {1,0,1},
            {1,1,1}
            };

        int iconNoteEdit[5][3] = {
            {1,1,1},
            {0,0,0},
            {1,1,1}
            };
        
        int iconSave[5][3] = {
            {0,1,0},
            {0,1,0},
            {1,1,1}
            };
};

#endif