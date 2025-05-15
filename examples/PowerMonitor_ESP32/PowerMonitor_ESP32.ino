//ESP32 Power Monitor
//ESP32 reads the from the scratchpad of ATtiny85

#include <Espresso.h>
#include <E2B.h>

#define E2B_pin 21

unsigned char rom[8] = {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
unsigned char scratchpad[9] = {0x00, 0x00, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};

unsigned long lastMillis = 0;
float ampHours = 0.0;
float wattHours = 0.0;

EspressoCM cm;
E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

//uint8_t KEY = 0x30; //CHANGE TO SECURED DEVICE KEY TO UNLOCK A SECURED DEVICE

void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Espresso Power Monitor - Host");

  //setHostFlag(rom,1);                 //Use when connecting multiple masters to the bus
  //e2b.setDeviceType(POINTTOPOINT);    //Can use this when only one device is connected
}

void loop(){
  byte i;
  byte present = 0;
  byte data[9];
  byte addr[8];

  unsigned long currentMillis = millis();
  float deltaTimeHours = (currentMillis - lastMillis) / 3600000.0; // convert ms to hours
  lastMillis = currentMillis;
  
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
  present = e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.write(0xBE);         // Get current data from power monitor

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for (i=0; i < 8; i++) {           // we need 9 bytes
    data[i] = e2b.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(E2B::crc8(data, 8), HEX);
  Serial.println();

  byte configByte = data[0];
  float current = data[1] + (data[2] / 100.0);
  float loadPower = data[3] + (data[5] / 100.0);
  float inputPower = data[6] + (data[7] / 100.0);
  float VIN = inputPower / current;

  float heatDissipation = inputPower - loadPower;
  float tempRise = 25 + (19.9 * heatDissipation);  // Or call getRegulatorTemperatureRise()
  ampHours += current * deltaTimeHours;
  wattHours += inputPower * deltaTimeHours;

  // Print the reassembled values
  Serial.print("Config Byte: "); Serial.println(configByte, HEX);
  Serial.print("Current = "); Serial.print(current); Serial.println(" A");
  Serial.print("Load Power = "); Serial.print(loadPower); Serial.println(" W");
  Serial.print("Input Power = "); Serial.print(inputPower); Serial.println(" W");
  Serial.print("Input Voltage = "); Serial.print(VIN); Serial.println(" V");
  Serial.print("Heat Dissipation = "); Serial.print(heatDissipation); Serial.println(" W");
  Serial.print("Regulator Temperature Rise = "); Serial.print(tempRise); Serial.println(" °C");
  Serial.print("Amp-Hours = "); Serial.print(ampHours, 4); Serial.println(" Ah");
  Serial.print("Watt-Hours = "); Serial.print(wattHours, 4); Serial.println(" Wh");

  delay(1000);
}