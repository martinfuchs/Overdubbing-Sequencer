#ifndef States_h
#define States_h

#include <Arduino.h>
#include <LedMatrix.h>

#define MAX_SEQUENCES_SELECT 10

class States
{
    public:
        enum MAIN_STATES{
            MAIN_PLAY,
            MAIN_NOTEEDIT,
            MAIN_SAVE,
            MAIN_END
        };

        // UI
        enum MENU1_SELECTION{
            MENU1_RECORD,
            MENU1_OUTPUT,
            MENU1_CHANNEL,
            MENU1_SEQUENCE,
            //MENU1_NOTES,
            MENU1_END
        };

        // OUTPUT STATES
        enum OUTPUT_TYPE{
            VOUT_GATE1,
            VOUT_GATE2,
            VOUT_PRESSURE,
            VOUT_END
        };

        enum CHANNEL_TYPE{
            CHANNEL_GATE1,
            CHANNEL_GATE2,
            CHANNEL_PRESSURE,
            CHANNEL_END
        };

        States();
        void update();

        void setNextMainState();
        void setPrevMainState();
        void setMainState(int newState);
        int getMainState();
        int getPrevMainState();

        void setNextMenu1Selection();
        void setPrevMenu1Selection();
        int getMenu1State();

        void selectNextChannel();
        void selectPrevChannel();
        int getSelectedChannel();

        void selectNextVout();
        void selectPrevVout();
        int getSelectedVout();

        bool getRecording();
        void setRecording(bool value);

        int getCurrentSelectedSequence();
    
    private:
        void setup();
        void mainStateChanged(int newState);

        bool recording = true;
        int currentMainState = MAIN_PLAY;
        int prevMainState = MAIN_PLAY;

        int currentMenu1Selection = MENU1_SEQUENCE;
        int currentVout = VOUT_GATE1;
        int selectedChannel = CHANNEL_GATE1;

        int currentSelectedSequence = 0;

};

#endif