//ESP32 Power Monitor
//ESP32 reads the from the scratchpad of ATtiny85

#include <E2B.h>

#define E2B_pin 2

unsigned char rom[8] = {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
unsigned char scratchpad[9] = {0x00, 0x00, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};

E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

//uint8_t KEY = 0x30; //CHANGE TO SECURED DEVICE KEY TO UNLOCK A SECURED DEVICE

void setup(){
  attachInterrupt(E2B_pin,respond,CHANGE);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("E2B Master Node Test.");
  e2b.init(rom);
  e2b.setScratchpad(scratchpad);

  //setHostFlag(rom,1);                 //Use when connecting multiple masters to the bus
  //e2b.setDeviceType(POINTTOPOINT);    //Can use this when only one device is connected
}

void respond(){
  e2b.MasterResetPulseDetection();
}

void loop(){
  byte i;
  byte present = 0;
  byte data[9];
  byte addr[8];
  
  //Searches for device
  //This section is not neededd if only one device is connected
  if(!e2b.search(addr)){
    Serial.println("No more addresses.");
    Serial.println();
    e2b.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for(i=0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (E2B::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();

  //Transmits ddata
  e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.write(0x44,1);        // start conversion, with parasite power on at the end
  
  delay(1000);
  
  present = e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.write(0xBE);         // Read Scratchpad

  /*Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for (i=0; i < 9; i++) {           // we need 9 bytes
    data[i] = e2b.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(E2B::crc8(data, 8), HEX);
  Serial.println();*/

  float integer = data[0];
  float decimal = data[1];
  decimal /= 100;
  float raw = integer + decimal;
  Serial.print("  Power = "); Serial.print(raw); Serial.println("W");
  
}
