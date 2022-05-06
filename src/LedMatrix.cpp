#include "LedMatrix.h"


LedMatrix::LedMatrix() {
    setup();
}


void LedMatrix::setup() {

  pinMode(EXTRA_PIN, OUTPUT); 
  pinMode(LATCH, OUTPUT); 
  digitalWrite(LATCH, HIGH);

  //SPI.begin();
  //SPI.setDataMode(SPI_MODE0);
  //SPI.setBitOrder(MSBFIRST);
  //SPI.setClockDivider(SPI_CLOCK_DIV128);

  registerPatterns = new byte[REGISTERS];
  clearRegisters();
}


void LedMatrix::writeRowToBytes(size_t row[RES_X], size_t _y){
    for(size_t x = 0; x<RES_X; x++){
      writePinToByte(xPins[x], row[x]);
    }

    for(size_t y = 0; y<RES_Y; y++){
      bool yVal = yValue[y];
      if(_y==y){
        yVal = !yVal;
      }
      writePinToByte(yPins[y], yVal);
    }
}


void LedMatrix::writePosToBytes(size_t _x, size_t _y){
    for(size_t x = 0; x<RES_X; x++){
      bool xVal = xValue[x];
      if(_x==x){
        xVal = !xVal;
      }
      writePinToByte(xPins[x], xVal);
    }
    for(size_t y = 0; y<RES_Y; y++){
      bool yVal = yValue[y];
      if(_y==y){
        yVal = !yVal;
      }
      writePinToByte(yPins[y], yVal);
    }
}


// Takes the overall pin number and figures out which register it's on and what pin it would be on that register
void LedMatrix::writePinToByte(size_t pin, bool state) {

  if(pin == -1){ // EXTRA_PIN
    extraPinValue = state;
    return;
  }

  // Which register are we looking at? 0=first, 1=second, etc. "/" will round down to an size_t
  size_t r = pin / 8;
  // Which pin on that register, finding the remainder of the pin division will give us 0-7
  size_t q = pin % 8;
  // Go through each of our stored patterns and write them back to the registers in order
  for (size_t i = 0; i < REGISTERS; i++) {
    // Posize_t to the current pattern for this register
    byte * pattern = &registerPatterns[i];
    // If this is the register that the pin we are editing is on...
    if (i == r) {
      // Take the "pattern" that holds the 8 bits, change the binary digit at position "q" (0-7) to "state" (LOW = 0, HIGH = 1)
      bitWrite(*pattern, q, state);
    }
  }
}


void LedMatrix::writeBytesToSPI(){

    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV128, MSBFIRST, SPI_MODE0));
    digitalWrite(LATCH, LOW);
    for (size_t i = 0; i < REGISTERS; i++) {
      // Posize_t to the current pattern for this register
      byte* pattern = &registerPatterns[i];
      // Write the pattern back to the register chips byte by byte
      SPI.transfer(*pattern);
    }
    digitalWrite(LATCH, HIGH);
    SPI.endTransaction();

    // EXTRA_PIN
    digitalWrite(EXTRA_PIN, extraPinValue);
}


void LedMatrix::clearRegisters() {
  // For each register, set it's pattern to 0 (which, rember is 00000000 in bytes, meaning all lights off)
  for (size_t i = 0; i < REGISTERS; i++) {
    registerPatterns[i] = 0;
  }

  extraPinValue = 0;
}
