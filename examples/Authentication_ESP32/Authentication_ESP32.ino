//Espresso E2B Authentication - Master example
/*NOTES:
*/
#include <E2B.h>

#define CMD_GET_UID           0x60
#define CMD_CHALLENGE         0x61
#define CMD_AUTH_TEST         0x62
#define CMD_READ_CONFIG       0x63
#define CMD_WRITE_CONFIG      0x64
#define CMD_LOCK_CHIP         0x65
#define CMD_GET_USAGE_COUNTER 0x66
#define CMD_ECHO              0x67
#define CMD_RESET_STATE       0x68

#define E2B_pin 21

unsigned char rom[8] = {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
unsigned char scratchpad[9] = {0x00, 0x00, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};

E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

// ----- Configurable Test Parameters -----
bool sendValidRequest = true;
bool sendMalformed = false;
bool spamRequests = false;

unsigned long lastSendTime = 0;
const unsigned long SPAM_INTERVAL = 200; // ms between spammy packets

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Espresso E2B Authentication - Host");
  //setHostFlag(rom,1);                 //Use when connecting multiple masters to the bus
  //e2b.setDeviceType(POINTTOPOINT);    //Can use this when only one device is connected
}

void loop(){
  byte addr[8];

  /*if(spamRequests && millis() - lastSendTime >= SPAM_INTERVAL) {
    lastSendTime = millis();
    sendRequest("AUTH_TEST", "hi", "0000"); // Intentionally bad sig
  }*/
  
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
  for(int i=0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (E2B::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  /*uint8_t packet1[9] = {CMD_GET_UID, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xA2, 0xF9};
  uint8_t packet2[9] = {CMD_WRITE_CONFIG, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xA2, 0xE1};
  sendRequest_E2B(addr, packet1);    //Valid message
  sendRequest_E2B(addr, packet2);    //Invalid message*/
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  uint8_t packet_uid[9]           = {CMD_GET_UID, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_challenge[9]     = {CMD_CHALLENGE, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};  // nonce = 0x1234
  uint8_t packet_auth_test[9]     = {CMD_AUTH_TEST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_read_config[9]   = {CMD_READ_CONFIG, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_write_config[9]  = {CMD_WRITE_CONFIG, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};  // write config = 0x42
  uint8_t packet_lock_chip[9]     = {CMD_LOCK_CHIP, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_usage_counter[9] = {CMD_GET_USAGE_COUNTER, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_echo[9]          = {CMD_ECHO, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0xA2, 0xF9};
  uint8_t packet_reset_state[9]   = {CMD_RESET_STATE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xF9};
  sendRequest_E2B(addr, packet_uid);
  sendRequest_E2B(addr, packet_challenge);
  sendRequest_E2B(addr, packet_auth_test);
  sendRequest_E2B(addr, packet_read_config);
  sendRequest_E2B(addr, packet_write_config);
  sendRequest_E2B(addr, packet_lock_chip);
  sendRequest_E2B(addr, packet_usage_counter);
  sendRequest_E2B(addr, packet_echo);
  sendRequest_E2B(addr, packet_reset_state);
  //////////////////////////////////////////////////////////////////////////////////////////////////////
}




void sendRequest_E2B(byte addr[8], uint8_t packet[9]){
  int i;
  byte present = 0;
  byte data[9];
  
  //Transmits data
  e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.write_scratchpad();
  for(i=0; i < 9; i++){
    e2b.write(rom[i]);
  }
  
  delay(1000);

  //Transmits ddata
  e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.write_scratchpad();
  for(i=0; i < 9; i++){
    e2b.write(packet[i]);
  }
  
  delay(1000);
  
  present = e2b.reset();
  e2b.select(addr);
  //e2b.unlock(KEY);        // uncomment when addressing secured devices
  e2b.read_scratchpad();         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for (i=0; i < 9; i++) {           // we need 9 bytes
    data[i] = e2b.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(E2B::crc8(data, 8), HEX);
  Serial.println();

  delay(1000);
}