#ifndef TouchPads_h
#define TouchPads_h

#include <Array.h>

#include <Arduino.h>
#include "Adafruit_MPR121.h"

#define NUM_TOUCHPADS 12

class TouchPads
{
    public:
        enum TOUCH_STATES{
            TOUCH_PRESSED,
            TOUCH_RELEASED,
            TOUCH_PENDING,
            TOUCH_NONE,
            TOUCH_END
        };

        TouchPads();

        typedef struct Touch {
            uint8_t touchPadId = 0;
            uint8_t eventType = TOUCH_NONE; // pending = 2, pressed = 1, released = -1, none = 0
            uint8_t pressure = 0;
        };

        void setup();
        void update();
        boolean hasNewInput();
        Array<TouchPads::Touch*,NUM_TOUCHPADS> getTouches();
    
    private:
        uint8_t getPressureValue(uint8_t padId);

        // You can have up to 4 on one i2c bus but one is enough for testing!
        Adafruit_MPR121 cap = Adafruit_MPR121();
        // Keeps track of the last pins touched
        // so we know when buttons are 'released'
        uint16_t lasttouched = 0;
        uint16_t currtouched = 0;

        Array<TouchPads::Touch*, NUM_TOUCHPADS> activeTouches;
        Array<TouchPads::Touch*, NUM_TOUCHPADS> prevTouches;
        
        bool hasTouch = false;
};


#endif