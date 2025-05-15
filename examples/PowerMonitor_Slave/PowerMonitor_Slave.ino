//Espresso Power Monitor
//The board reads the current output from onboard op-amp and saves value to scratchpad

#include <E2B.h>

#define E2B_pin 2
#define LEDpin_G 3
#define LEDpin_Y 4

E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

byte configByte = 0xAA;

float VIN;

//unsigned char rom[8];
unsigned char rom[8] = {FAMILYCODE, 0xE2, 0xCC, 0x2D, 0x01, 0x25, 0xB8, 0x30};
unsigned char scratchpad[9] = {configByte, 0x00, 0x00, 0x04, 0x00, 0x31, 0x6A, 0xD7, 0x00};

void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Espresso Power Monitor.");
  attachInterrupt(E2B_pin,respond,CHANGE);
  e2b.init(rom);
  e2b.setScratchpad(scratchpad);

  pinMode(LEDpin_G,OUTPUT);
  pinMode(LEDpin_Y,OUTPUT);
}

void respond(){
  e2b.MasterResetPulseDetection();
}

void loop(){
  VIN = getInputVoltage();
  getPower();
  e2b.waitForRequest(false);
}

//Assembles the input voltage
//This is a placeholder for users to add their own retreival logic
float getInputVoltage(){
  return 11.1;
}

//Assembles data values into the scratchpad
void getPower(){
  int IntegerPart, DecimalPart;
  float current = 0.53; //cm.getBoardCurrent();
  float loadPower = 3.3 * current;
  //Serial.print("Load Power = "); Serial.println(loadPower);

  IntegerPart = (int)(current);
  DecimalPart = 100 * (current - IntegerPart);
  e2b.scratchpad[1] = IntegerPart;
  e2b.scratchpad[2] = DecimalPart;
  
  IntegerPart = (int)(loadPower);
  DecimalPart = 100 * (loadPower - IntegerPart);
  e2b.scratchpad[3] = IntegerPart;
  e2b.scratchpad[5] = DecimalPart;

  float inputPower = VIN * current;
  //Serial.print("Input Power = "); Serial.println(inputPower);
  IntegerPart = (int)(inputPower);
  DecimalPart = 100 * (inputPower - IntegerPart);
  e2b.scratchpad[6] = IntegerPart;
  e2b.scratchpad[7] = DecimalPart;

  e2b.scratchpad[8] = 0x0;

  //Serial.println();

  if(loadPower >= 4.5){
    digitalWrite(LEDpin_Y,HIGH);
  }else{
    digitalWrite(LEDpin_Y,LOW);
  }
}