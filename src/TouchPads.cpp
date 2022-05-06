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
    for (uint8_t i=0; i<NUM_TOUCHPADS; i++) {

        activeTouches[i]->touchPadId = i;
        //activeTouches[i].eventType = 0;
        // it if *is* touched and *wasnt* touched before, alert!
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
            activeTouches[i]->eventType = TOUCH_PRESSED;
            //activeTouches[i].pressure = 125;
            //int pressureValue = cap.filteredData(i);
            //activeTouches[i].pressure.add(pressureValue);
            hasTouch = true;
            count = 0;
        }
        // if it *was* touched and now *isnt*, alert!
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
            activeTouches[i]->eventType = TOUCH_RELEASED;
            hasTouch = true;
        }

        if(activeTouches[i]->eventType == TOUCH_PENDING){
            int pressureValue = cap.filteredData(i); // 125 no input -> 0 input (minimum ca. 50)
            activeTouches[i]->pressure = pressureValue;
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
      //activeTouches[i].pressure.erase();
    }
  }
  return prevTouches;
}

