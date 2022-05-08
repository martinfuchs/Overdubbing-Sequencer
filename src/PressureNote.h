#ifndef Pressure_h
#define Pressure_h

#include <ustd_array.h>


typedef struct PressureNote {
    uint16_t maxLength = 420;
    uint16_t recordPosition = 0;
    uint16_t playbackPosition = 0;
    ustd::array<uint8_t> pressure = ustd::array<uint8_t>(maxLength, maxLength, 0, false);
};

#endif