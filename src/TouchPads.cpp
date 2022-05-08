#include "TouchPads.h"

TouchPads::TouchPads() {
}

void TouchPads::setup(){
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  //Serial.println("MPR121 found!");

  for(uint8_t i=0; i<NUM_TOUCHPADS; i++){
    Touch* newT = new Touch();
    newT->touchPadId = i;
    activeTouches[i] = newT;
  }

  for(uint8_t i=0; i<NUM_TOUCHPADS; i++){
    Touch* newT = new Touch();
    newT->touchPadId = i;
    prevTouches[i] = newT;
  }
}


int count = 0;
void TouchPads::update(){

    // Get the currently touched pads
    currtouched = cap.touched();
    hasTouch = false;
    //Serial.println(getPressureValue(0));
    for (uint8_t i=0; i<NUM_TOUCHPADS; i++) {

        activeTouches[i]->touchPadId = i;
        //activeTouches[i].eventType = 0;
        // it if *is* touched and *wasnt* touched before, alert!
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) { // -->> todo: fix: sometimes not trigger
            activeTouches[i]->eventType = TOUCH_PRESSED;
            activeTouches[i]->pressure = 0; //reset
            hasTouch = true;
            count = 0;
        }
        // if it *was* touched and now *isnt*, alert!
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
            activeTouches[i]->eventType = TOUCH_RELEASED;
            activeTouches[i]->pressure = 0; //reset
            hasTouch = true;
        }

        if(activeTouches[i]->eventType == TOUCH_PENDING){
            activeTouches[i]->pressure = getPressureValue(i);
            hasTouch = true;
            count ++;
        }

    }

    // reset our state
    lasttouched = currtouched;
  
  // debugging info, what
  // Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  // Serial.print("Filt: ");
  // for (uint8_t i=0; i<12; i++) {
  //   Serial.print(cap.filteredData(i)); 
  //   Serial.print("\t");
  // }
  // Serial.println();
  // Serial.print("Base: ");
  // for (uint8_t i=0; i<12; i++) {
  //   Serial.print(cap.baselineData(i)); 
  //   Serial.print("\t");
  // }

}

uint8_t TouchPads::getPressureValue(uint8_t padId){
    uint8_t pressureValue = cap.filteredData(padId); // 125 no input -> 0 input (minimum ca. 50)
    return map(pressureValue,125,50,0,255);
}


boolean TouchPads::hasNewInput(){
  return hasTouch;
}

Array<TouchPads::Touch*,NUM_TOUCHPADS> TouchPads::getTouches(){
  for (uint8_t i=0; i<NUM_TOUCHPADS; i++) {

    // copy to prev
    prevTouches[i]->pressure = activeTouches[i]->pressure;
    prevTouches[i]->eventType = activeTouches[i]->eventType;

    // change from pressed to pending / released to none
    if(activeTouches[i]->eventType == TOUCH_PRESSED){
      activeTouches[i]->eventType = TOUCH_PENDING;
    }
    if(activeTouches[i]->eventType == TOUCH_RELEASED){
      activeTouches[i]->eventType = TOUCH_NONE;
    }
  }
  return prevTouches;
}

