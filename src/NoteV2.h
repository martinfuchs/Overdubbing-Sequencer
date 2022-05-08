#ifndef NoteV2_h
#define NoteV2_h

#include <Arduino.h>

class NoteV2
{
  public:
    NoteV2();
    void reset();
    void update(int time);

    void startRecord(uint8_t time, uint8_t _index, uint8_t _padId);
    void endRecord(int time);

    uint8_t getOutput();
    void setDisabled(bool value);
    bool getDisabled();
    uint8_t getIndex();
    uint8_t getStartTime();
    uint8_t getEndTime();
    uint8_t getPadId();

  private:
    void setup();
    void startPlay();
    void endPlay();

    uint8_t index = 0;
    uint8_t padId = 0;

    int startTime = 0;
    int endTime = 0;

    uint16_t duration = 0;
    bool pendingRecord = false;
    bool pendingPlay = false;

    bool disabled = true;

    uint8_t output = 0;

};
#endif