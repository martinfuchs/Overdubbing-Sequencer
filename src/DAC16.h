
#ifndef DAC16_H
#define DAC16_H

#include "Arduino.h"
#include <SPI.h>

class DAC16
{ 
public:
  DAC16();
  void setup();
  void write(unsigned value);
  void hide();
  
private:
  const byte SYNCpin = 9;
};

#endif