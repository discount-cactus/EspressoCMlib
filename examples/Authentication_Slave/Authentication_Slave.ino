//Espresso CM Authentication Program
/*NOTES:
MEMORY MAP:
Address: Range	Size	Purpose	Description
0x00–0x07:   8B    ROM / Device UID	Unique 8-byte ID for the authentication chip (used in rom[])
0x08–0x0F:   8B    (Reserved)	Reserved for future device info (e.g. manufacture date, checksum, etc.)
0x10:        1B    Config Byte	User-defined config flags or settings (set/read via CMD_WRITE_CONFIG)
0x11:        1B    Lock Flag	Non-zero value = chip is locked (set via CMD_LOCK_CHIP)
0x12–0x1F:   14B   (Reserved)	Reserved for future secure state, keys, etc.
0x20–0x21:   2B    Usage Counter	16-bit usage counter (read via CMD_GET_USAGE_COUNTER)
0x22–0xFF:   ~222B (Available / Reserved)	Free for future features like temporary secrets, audit logs, pairing data

*/
#include <E2B.h>
//#include <avr/wdt.h>
#include <EEPROM.h>

#define E2B_pin 2

const unsigned char romDatabase[3][8] = {
  {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00},
  {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00},
  {FAMILYCODE, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00}
};

const uint8_t secret = 0xA2F9;

unsigned char rom[8] = {FAMILYCODE, 0xE2, 0xCC, 0x2D, 0x01, 0x25, 0xB8, 0x30};
unsigned char scratchpad[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

bool hasReceivedAddress = 0;
bool unlockState = 0;

E2B e2b(E2B_pin);  // on pin 2 (a 4.7K resistor is necessary)

// 💡 Rate Limiting Parameters
const unsigned long RATE_LIMIT_INTERVAL = 500;  // ms
unsigned long lastRequestTime = 0;

const uint8_t DEVICE_ID_ADDR = 0x00;            // 🆔 Device ID location in EEPROM

void setup() {
  attachInterrupt(E2B_pin,respond,CHANGE);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Espresso E2B Authentication - Slave");
  e2b.init(rom);
  e2b.setScratchpad(scratchpad);

  readDeviceID();             // 🆔 Get Unique ID
  //wdt_enable(WDTO_1S);        // ⏱️ Enable watchdog timer
}

void respond(){
  e2b.MasterResetPulseDetection();
}

void loop() {
  //wdt_reset(); // ⏱️ Watchdog: Reset each loop
  
  e2b.waitForRequest(false);
  uint8_t raw = e2b.getScratchpad(4);
  //Serial.println(raw,HEX);
  if(raw == 0xF){
    //Validates the address
    if(!hasReceivedAddress){
      if(addressInDatabase()){
        unlockState = 1;
        hasReceivedAddress = true;
        //Serial.println("Valid address");
      }else{
        unlockState = 0;
        //Serial.println("Invalid address");
      }
    }
  }else if(raw == 0x0){
    //Handles packet data
    if((hasReceivedAddress) && (unlockState)){
      handleIncomingMessage();  // 🔐 Main message handler

      hasReceivedAddress = 0;   //Resets for next request loop
      unlockState = 0;          //Resets for next request loop
    }
  }
}





//Checks that the device address is in the database
//Ensures no external or unknown devices are trying to communicate with the authentication chip
bool addressInDatabase(){
  bool match;
  int i,j;
  for (i=0; i < 3; i++){
    match = 1;
    for(j=0; j < 8; j++){
      if(e2b.getScratchpad(j) != romDatabase[i][j]){
        match = 0;
      }
    }
    if(match){
      return 1;
    }
  }
  return 0;
}

// 🔐 Main function to handle messages and validate/authenticate
bool handleIncomingMessage() {
  // 🛡️ Feature: Rate Limiting
  /*if (!checkRateLimit()) {
    return 0;
  }*/

  // 🚫 Reject obviously malformed requests
  if ((e2b.getScratchpad(7) == highByte(secret)) && (e2b.getScratchpad(8) == lowByte(secret))) {
    return 0;
  }

  // 🔁 Shared scratch variables
  uint8_t i;
  uint8_t config;
  uint16_t count;
  uint16_t nonce, sig;
  // 🎯 Handle known commands
  switch(e2b.getScratchpad(0)){
    // 📌 CMD_GET_UID (0x60)
    case 0x60:
      for(i=0; i < 4; i++) {
        scratchpad[i] = rom[i];
        scratchpad[i+5] = rom[i+4];
      }
      e2b.setScratchpad(scratchpad);
      break;
    // 🔐 CMD_CHALLENGE (0x61)
    case 0x61:
      nonce = ((uint16_t)e2b.getScratchpad(1) << 8) | e2b.getScratchpad(2);
      sig = nonce ^ secret;
      scratchpad[0] = highByte(sig);
      scratchpad[1] = lowByte(sig);
      for(i=2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // 🧪 CMD_AUTH_TEST (0x62)
    case 0x62:
      scratchpad[0] = 'O';
      scratchpad[1] = 'K';
      for(i=2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // ⚙️ CMD_READ_CONFIG (0x63)
    case 0x63:
      config = EEPROM.read(0x10);
      scratchpad[0] = config;
      for(i=1; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // ✍️ CMD_WRITE_CONFIG (0x64)
    case 0x64:
      config = e2b.getScratchpad(1);
      EEPROM.write(0x10, config);
      scratchpad[0] = 'W';
      scratchpad[1] = 'R';
      for(i=2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // 🔐 CMD_LOCK_CHIP (0x65)
    case 0x65:
      EEPROM.write(0x11, 1);  // Lock flag
      scratchpad[0] = 'L';
      scratchpad[1] = 'K';
      for(i=2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // 🔢 CMD_GET_USAGE_COUNTER (0x66)
    case 0x66:
      count = (EEPROM.read(0x20) << 8) | EEPROM.read(0x21);
      scratchpad[0] = highByte(count);
      scratchpad[1] = lowByte(count);
      for(i=2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // 🔁 CMD_ECHO (0x67)
    case 0x67:
      for(i=0; i < 4; i++) {
        scratchpad[i] = e2b.getScratchpad(i + 1); // Echo back bytes 1–4
      }
      for(i=4; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // 🧼 CMD_RESET_STATE (0x68)
    case 0x68:
      hasReceivedAddress = 0;
      unlockState = 0;
      scratchpad[0] = 'R';
      scratchpad[1] = 'S';
      for (i = 2; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
    // ❓ Unknown command
    default:
      scratchpad[0] = 'E';
      scratchpad[1] = 'R';
      scratchpad[2] = 'R';
      for(i=3; i < 8; i++) scratchpad[i] = 0x00;
      e2b.setScratchpad(scratchpad);
      break;
  }

  return 1;
}

// 🆔 Feature: Read unique device ID from EEPROM
void readDeviceID() {
  //Initializes if not initialized before
  //if(!EEPROM.read(DEVICE_ID_ADDR)){
    //randomSeed(analogRead(0));          //Uncomment when generating new rom address
    //e2b.generateROM(rom);               //Uncomment when generating new rom address
    for (uint8_t i=0; i < 8; i++) {
      EEPROM.write(DEVICE_ID_ADDR + i,rom[i]);
    }
  //}

  //Reads ID from EEPROM
  for (uint8_t i=0; i < 8; i++) {
    //deviceID[i] = EEPROM.read(DEVICE_ID_ADDR + i);
    rom[i] = EEPROM.read(DEVICE_ID_ADDR + i);
  }
  /*for(uint8_t i=0; i < 8; i++) {
    Serial.print(rom[i],HEX); Serial.print(" ");
  }
  Serial.println(" ");*/
}

// 🛡️ Feature: Rate Limiting to prevent DoS/flooding
bool checkRateLimit() {
  unsigned long now = millis();
  if (now - lastRequestTime < RATE_LIMIT_INTERVAL) {
    return false;
  }
  lastRequestTime = now;
  return true;
}