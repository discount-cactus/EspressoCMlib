//ATtiny85 Power Monitor
//ATtiny85 reads the current output from onboard op-amp and saves value to scratchpad

#include "Espresso.h"
#include <E2B.h>

#define p_threshold 5  //5 watts

EspressoCM cm;
E2B e2b(E2B_BUILTIN);  // on pin 2 (a 4.7K resistor is necessary)

//unsigned char rom[8];
unsigned char rom[8] = {FAMILYCODE, 0xE2, 0xCC, 0x2D, 0x01, 0x25, 0xB8, 0x30};
unsigned char scratchpad[9] = {0x5E, 0x00, 0x00, 0x04, 0x00, 0x31, 0x6A, 0xD7, 0x00};
 
void getPower(){
  float voltage = 3.3;
  float current = cm.getBoardCurrent();
  float power = voltage * current;
  
  int IntegerPart = (int)(power);
  int DecimalPart = 100 * (power - IntegerPart);
  e2b.scratchpad[0] = IntegerPart;
  e2b.scratchpad[1] = DecimalPart;

  if(power >= p_threshold){
    digitalWrite(LEDpin,HIGH);
  }else{
    digitalWrite(LEDpin,LOW);
  }
}

void setup(){
    attachInterrupt(E2B_BUILTIN,respond,CHANGE);
    e2b.init(rom);
    e2b.setScratchpad(scratchpad);

    pinMode(LEDpin,OUTPUT);
}

void respond(){
  e2b.MasterResetPulseDetection();
}

void loop(){
  getPower();
  e2b.waitForRequest(false);
}
