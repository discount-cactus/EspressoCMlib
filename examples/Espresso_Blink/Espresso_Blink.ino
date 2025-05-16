//Espresso SW library test
#include "Espresso.h"

EspressoCM cm;

void setup() {
  pinMode(cm.LEDpin,OUTPUT);
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Espresso Blink");

  Serial.print("Core: "); Serial.println(cm.getCoreNumber());
}

void loop() {
  digitalWrite(cm.LEDpin,HIGH);
  delay(1000);
  digitalWrite(cm.LEDpin,LOW);
  delay(1000);
}