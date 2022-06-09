
#include "DAC16.h"

DAC16::DAC16(){ 
  setup();
}

void DAC16::setup(){
    pinMode(SYNCpin, OUTPUT);
    digitalWrite(SYNCpin, HIGH);
}

void DAC16::write(unsigned value)
{ 
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE1));
  digitalWrite(SYNCpin, LOW);

  byte highByte = (value >> 8);
  byte lowByte = (value & 0xFF);

  SPI.transfer(0);
  SPI.transfer(highByte);
  SPI.transfer(lowByte);

  digitalWrite(SYNCpin, HIGH);
  SPI.endTransaction();
}


void DAC16::hide(){
  // maximum voltage -> lowest tone
  write(65535);
}