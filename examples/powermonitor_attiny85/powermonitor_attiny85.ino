//Espresso Power Monitor - Attiny95
//The board reads the current output from onboard op-amp and saves value to scratchpad
#include <E2B.h>

#define E2B_pin 2
#define LEDpin_G 3
#define LEDpin_Y 1
#define VIN_pin 4

#define R_sense 0.003
#define opamp_gain 733.33
#define v_offset 0.05

E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

byte configByte = 0xAA;


//unsigned char rom[8];
unsigned char rom[8] = {FAMILYCODE, 0xE2, 0xCC, 0x2D, 0x01, 0x25, 0xB8, 0x30};
unsigned char scratchpad[9] = {configByte, 0x00, 0x00, 0x04, 0x00, 0x31, 0x6A, 0xD7, 0x00};

void setup(){
  attachInterrupt(E2B_pin,respond,CHANGE);
  e2b.init(rom);
  e2b.setScratchpad(scratchpad);

  pinMode(LEDpin_G,OUTPUT);
  pinMode(LEDpin_Y,OUTPUT);
  pinMode(VIN_pin,INPUT);
}

void respond(){
  e2b.MasterResetPulseDetection();
}

void loop(){
  digitalWrite(LEDpin_G,HIGH);
  e2b.waitForRequest(false);
  digitalWrite(LEDpin_G,LOW);
  if (e2b.getScratchpad(4) != 0xBE || e2b.getScratchpad(0) == 0xA){
    getPower();
  }
}

//Assembles the input voltage
//This is a placeholder for users to add their own retreival logic
float getInputVoltage(){
  return 5.0;
}

//Assembles data values into the scratchpad
void getPower(){
  int IntegerPart, DecimalPart;

  float VIN = getInputVoltage();

  float raw = analogRead(VIN_pin);
  float voltage = (raw / 1023.0) * 3.3;   //Scales to actual voltage (assuming 3.3V Vcc as ADC ref)
  voltage -= v_offset;                    //Calculates out the offset from the PCB trace
  float v_sense = voltage / opamp_gain;   //Back-calculates with the op-amps gain
  float current = v_sense / R_sense;      //Uses Ohm's Law to calculate the measured current from the shunt resistor

  float loadPower = 3.3 * current;
  e2b.scratchpad[0] = configByte;

  IntegerPart = (int)(current);
  DecimalPart = 100 * (current - IntegerPart);
  e2b.scratchpad[1] = IntegerPart;
  e2b.scratchpad[2] = DecimalPart;
  
  IntegerPart = (int)(loadPower);
  DecimalPart = 100 * (loadPower - IntegerPart);
  e2b.scratchpad[3] = IntegerPart;
  e2b.scratchpad[5] = DecimalPart;

  float inputPower = VIN * current;
  IntegerPart = (int)(inputPower);
  DecimalPart = 100 * (inputPower - IntegerPart);
  e2b.scratchpad[6] = IntegerPart;
  e2b.scratchpad[7] = DecimalPart;

  e2b.scratchpad[8] = raw;

  if(loadPower >= 4.5){
    digitalWrite(LEDpin_Y,HIGH);
  }else{
    digitalWrite(LEDpin_Y,LOW);
  }
}