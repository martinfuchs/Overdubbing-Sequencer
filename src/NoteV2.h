#ifndef NoteV2_h
#define NoteV2_h

#include <Arduino.h>
#include <Array.h>


#define PRESSURE_LENGTH 6

class NoteV2
{
  public:
    NoteV2();
    void reset();
    void update(int time);

    void startRecord(int time, int _index, int _padId, bool _printDebug = false);
    void endRecord(int time);

    int getOutput();
    void setDisabled(bool value);
    bool getDisabled();
    int getIndex();
    int getStartTime();
    int getEndTime();
    int getPadId();

  private:
    void setup();
    void startPlay();
    void endPlay();

    void __debugPrint(String message, bool newline = false);


    int index = 0;
    int padId = 0;

    int startTime = 0;
    int endTime = 0;

    int duration = 0;
    bool pendingRecord = false;
    bool pendingPlay = false;

    bool disabled = true;

    int output = 0;

    bool printDebug = false;

    //Array<int,PRESSURE_LENGTH> pressure;

};
#endif