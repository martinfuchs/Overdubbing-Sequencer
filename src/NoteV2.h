#ifndef NoteV2_h
#define NoteV2_h

#include <Arduino.h>
#include "PressureNote.h"


class NoteV2
{
  public:

    NoteV2();
    void reset();
    void update(uint32_t roundedTime);

    void startRecord(uint32_t roundedTime, uint8_t _index, uint8_t _padId);
    void endRecord(uint32_t roundedTime);

    void setPressureNote(PressureNote * _pNote);
    void recordPressure(uint32_t roundedTime, uint8_t pressure);
    uint8_t getPressureOutput();

    void createFromValues(uint8_t _index, uint8_t _padId, uint32_t startTime, uint16_t durationMillis);

    uint8_t getOutput();
    void setDisabled(bool value);
    bool getDisabled();
    uint8_t getIndex();
    uint8_t getPadId();
    uint8_t getStartTime();

    void setDurationMillis(uint16_t _durationMillis);
    uint16_t getDurationMillis();

    bool isPlaying();

  private:
    void setup();
    void startPlay();
    void endPlay();
    void playPressure(uint32_t roundedTime);

    uint8_t index = 0;
    uint8_t padId = 0;

    uint32_t startTime = 0;
    //uint32_t endTime = 0;
    uint32_t playbackPosition = 0;

    uint16_t durationMillis = 0;
    bool pendingRecord = false;
    bool pendingPlay = false;

    uint16_t lastUpdateTimeMillis = 0;

    bool disabled = true;

    uint8_t output = 0;

    bool usePressure = false;
    PressureNote * pNote;
};
#endif