#ifndef LedMatrix_h
#define LedMatrix_h

#include <Arduino.h>
#include <SPI.h>

#define RES_X 10
#define RES_Y 7

#define DATAOUT 11//MOSI
#define DATAIN 12//MISO
#define SPICLOCK 13//SCK
#define LATCH 4
#define EXTRA_PIN 3//DIRECT PIN

#define REGISTERS 2
#define PINS REGISTERS * 8

class LedMatrix
{
  public:
    LedMatrix();
    void writeRowToBytes(size_t row[RES_X], size_t _y);
    void writePosToBytes(size_t x, size_t y);
    void writeBytesToSPI();

  private:

    void setup();
    void clearRegisters();
    void writePinToByte(size_t pin, bool state);

    byte * registerPatterns;
    bool extraPinValue = 0;

    // Remap pins, because they are distributed on the hardware in strange order :/
    size_t yPins[RES_Y] = {8, 9, 10, 11, 12, 13, 14};
    bool yValue[RES_Y] = {true, true, true, true, true, true, true};
    size_t xPins[RES_X] = {-1, 15, 7, 6, 5, 4, 3, 2, 1, 0}; // -1 = EXTRA_PIN wired directly from teensy :/
    bool xValue[RES_X] = {false, false, false, false, false, false, false, false, false, false};

};
#endif