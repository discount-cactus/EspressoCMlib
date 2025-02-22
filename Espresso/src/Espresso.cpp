/*
Name:     Espresso.cpp
Created:  8/8/24 8:12:06 PM
Author:   Brandon
*/

#include "Espresso.h"



EspressoCM::EspressoCM(){
  CORE = 0;
  CORESEL0 = 0;
  CORESEL1 = 0;
  LEDpin = 0;
  E2B_BUILTIN = 0;
  E2B_EXTERNAL = 0;
  CS_FLASH = 0;
  LEDpin = 0;
  E2B_BUILTIN = 0;
  ISNS_pin = 0;

  #if ARDUINO_ARCH_ESP32
    CORESEL0 = 37;
    CORESEL1 = 36;
    LEDpin = 46;
    E2B_BUILTIN = 9;
    E2B_EXTERNAL = 21;
    CS_FLASH = 10;
    pinMode(CORESEL0,INPUT);
    pinMode(CORESEL1,INPUT);
    if(( digitalRead(CORESEL0) ) && ( !digitalRead(CORESEL1) )){
      CORE = CORE1;
    }
    if(( !digitalRead(CORESEL0) ) && ( digitalRead(CORESEL1) )){
      CORE = CORE2;
    }
    if (digitalRead(CORESEL0) == digitalRead(CORESEL1)){
      #warning "Invalid read of core assignment pins."
    }
  #elif __AVR_ATtiny85__
    LEDpin = 3;
    E2B_BUILTIN = 2;
    ISNS_pin = 4;
    CORE = CORE3;
  #else
    #error "Incorrect microcontroller core selected."
  #endif

  //String username = "";
  //String password = "";

}

//Returns the current draw of the Espresso CM.
uint8_t EspressoCM::getCoreNumber(){
  return CORE;
}

//Returns which supply is powering the CM
// Returns 1 if powered via battery, returns 0 if powered by main power connector(s)
/*bool EspressoCM::getPowerSource(){
  if(analogRead(VREF_VBATT_pin) > 10){
    return 1;
  }else{
    return 0;
  }
}*/

//Returns the current draw of the Espresso CM.
float EspressoCM::getBoardCurrent(){
  float offset_ISNS = 0.1;              //ISNS offset of 100mV due to resistance in the trace
  int value = analogRead(ISNS_pin);
  float current = value * 3.3/4095;
  current + offset_ISNS;

  return current;
}

//Computes and returns the power consumption of the Espresso CM board and writes to its scratchpad
//This function is made for use ONLY with the onboard ATtiny85 chip.
float EspressoCM::getBoardPower_ATtiny(){
  #if (__AVR_ATtiny85__)
    float voltage = 3.3;
    float current = getBoardCurrent();
    float power = voltage * current;

    int IntegerPart = (int)(power);
    int DecimalPart = 100 * (power - IntegerPart);
    scratchpad[0] = IntegerPart;
    scratchpad[1] = DecimalPart;

    return power;
  #else
    return 0;
  #endif
}

//Generates a random password.
/*String EspressoCM::generate_password(){
  String pwd = "";
  int l = random(8,16);
  String vals[88] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","1","2","3","4","5","6","7","8","9","0","!","@","#","$","%","^","&","*","(",")","-","_","=","+",",",".","<",">","/","?","[","]","{","}","\","|"};
  for(int j=0; j < l; j++){
    int a = random(88);
    String inc = vals[a];

    pwd += inc;
  }
  return password;
}*/

//Creates a private wi-fi server.
/*void EspressoCM::establish_WiFi_server(){
}*/

//Returns the input value in mils
//Converts an input value (in millimeters) to mils
float EspressoCM::mils(float val_mm){
  float num = val_mm * 39.37;
  return num;
}

//Returns the input value in millimeters
//Converts an input value (in mils) to millimeters
float EspressoCM::millimeters(float val_mils){
  float num = val_mils / 39.37;
  return num;
}

//Computes a standard XOR checksum
uint8_t EspressoCM::checksum(const uint8_t *addr, uint8_t len){
  uint8_t checksum = 0;
  for (int i=0; i < len; i++) checksum ^= addr[i];
	return checksum;
}
//Hamming Code Test Sketch
/*NOTES:
-This code demonstrates Hamming(7,4) encoding and decoding. You can modify it to suit your specific requirements.
-This implementation assumes a single-bit error. For multiple-bit errors, consider using more advanced error-correcting codes.
*//*

void setup() {
  Serial.begin(9600);
  while(!Serial){}

  uint8_t data = 0xA; // 1010 in binary
  uint8_t encodedData = hamming74Encode(data);
  uint8_t decodedData = hamming74Decode(encodedData);

  Serial.print("Input Data: "); Serial.println(data,HEX);
  Serial.print("Encoded Data: "); Serial.println(encodedData, BIN);
  Serial.print("Decoded Data: "); Serial.println(decodedData,HEX);
}

void loop(){
}*/

//Hamming(7,4) Encoding Function
uint8_t EspressoCM::hammingEncode(uint8_t data){
  // Ensure data is a 4-bit number (only the lower 4 bits of the input are used)
  data &= 0x0F;

  //Returns 0 if not a 4-bit number
  if(data < 8){
    return 0;
  }

  // Calculate the parity bits
  bool p1 = (data >> 3 & 1) ^ (data >> 2 & 1) ^ (data & 1); // p1: d1, d2, d4
  bool p2 = (data >> 3 & 1) ^ (data >> 1 & 1) ^ (data & 1); // p2: d1, d3, d4
  bool p3 = (data >> 2 & 1) ^ (data >> 1 & 1) ^ (data & 1); // p3: d2, d3, d4
  bool d1 = bitRead(data,3);
  bool d2 = bitRead(data,2);
  bool d3 = bitRead(data,1);
  bool d4 = bitRead(data,0);

  // Create the 7-bit encoded value:
  // p1 p2 d1 p3 d2 d3 d4
  uint8_t encodedData = 0x00;
  bitWrite(encodedData,0,p1); bitWrite(encodedData,1,p2); bitWrite(encodedData,2,d1); bitWrite(encodedData,3,p3);
  bitWrite(encodedData,4,d2); bitWrite(encodedData,5,d3); bitWrite(encodedData,6,d4); bitWrite(encodedData,7,0);

  return encodedData;
}

//Hamming(7,4) Decoding Function
uint8_t EspressoCM::hammingDecode(uint8_t encodedData){
    bool p1 = bitRead(encodedData,6);
    bool p2 = bitRead(encodedData,5);
    bool d1 = bitRead(encodedData,4);
    bool p3 = bitRead(encodedData,3);
    bool d2 = bitRead(encodedData,2);
    bool d3 = bitRead(encodedData,1);
    bool d4 = bitRead(encodedData,0);

    // Calculate parity check bits
    bool c1 = (d1 ^ d2 ^ d4) ^ p1; // Check parity 1
    bool c2 = (d1 ^ d3 ^ d4) ^ p2; // Check parity 2
    bool c3 = (d2 ^ d3 ^ d4) ^ p3; // Check parity 3

    // Determine the error position (if any)
    uint8_t errorPos = c1 * 1 + c2 * 2 + c3 * 4;

    // If errorPos is non-zero, there's an error at that bit position
    if (errorPos) {
        // Correct the error by flipping the bit at errorPos
        encodedData ^= (1 << (7 - errorPos));  // Flip the bit at the error position
    }

    // Extract the corrected data bits (d1, d2, d3, d4)
    /*uint8_t correctedData = ((encodedData >> 3) & 1) << 3 |  // d1
                            ((encodedData >> 2) & 1) << 2 |  // d2
                            ((encodedData >> 1) & 1) << 1 |  // d3
                            (encodedData & 1);               // d4*/
    uint8_t correctedData = 0x00;
    bitWrite(correctedData,0,(encodedData >> 3) & 1);
    bitWrite(correctedData,1,(encodedData >> 2) & 1);
    bitWrite(correctedData,2,(encodedData >> 1) & 1);
    bitWrite(correctedData,3,(encodedData & 1));

    return correctedData;
}

//Low-Density Parity Code Test Sketch
/*void setup(){
  Serial.begin(9600);

  byte originalData = 0xC7; // Example 8-bit data

  uint16_t encodedData = encodeLDPC(originalData);
  byte decodedData = decodeLDPC(encodedData);

  Serial.println("LDPC Encoding and Decoding Example:");
  Serial.print("Original Data: "); Serial.println(originalData, HEX);
  Serial.print("Encoded Data: "); Serial.println(encodedData, BIN);
  Serial.print("Decoded Data: "); Serial.println(decodedData, HEX);
}

void loop(){
}*/

// Function to encode a byte using LDPC
uint16_t EspressoCM::encodeLDPC(byte data){
  const int n = 8; // Number of data bits
  const int m = 12; // Number of total bits (including parity)
  const int parityCheckMatrix[m - n][n] = {
    {1, 1, 0, 0, 1, 0, 1, 1},  // Example LDPC parity-check matrix (4x8)
    {0, 1, 1, 0, 1, 1, 1, 0},
    {1, 0, 1, 1, 0, 1, 1, 0},
    {1, 1, 1, 0, 1, 1, 0, 0}
  };
  uint16_t encodedData = 0;

  // Store the original data bits in the first part of the encoded byte
  for (int i=0; i < n; i++){
    bitWrite(encodedData,i,bitRead(data,i));
  }

  // Calculate the parity bits and append them to the encoded byte
  for (int i=0; i < m - n; i++){
    bool parityBit = 0;
    for (int j=0; j < n; j++){
      parityBit += bitRead(data,i) * parityCheckMatrix[i][j];
    }
    // Place the parity bits in the remaining positions in encodedData
    encodedData |= (parityBit << (n + i));
  }

  return encodedData;
}

// Function to decode a byte using LDPC
byte EspressoCM::decodeLDPC(uint16_t encodedData){
  const int n = 8; // Number of data bits
  const int m = 12; // Number of total bits (including parity)
  const int parityCheckMatrix[m - n][n] = {
    {1, 1, 0, 0, 1, 0, 1, 1},  // Example LDPC parity-check matrix (4x8)
    {0, 1, 1, 0, 1, 1, 1, 0},
    {1, 0, 1, 1, 0, 1, 1, 0},
    {1, 1, 1, 0, 1, 1, 0, 0}
  };
  byte decodedData = 0;

  // Example hard-decision decoding (simplified)
  for (int i=0; i < n; i++){
    bool parityBit = 0;
    for (int j=0; j < m - n; j++){
      parityBit += bitRead(encodedData,i) * parityCheckMatrix[j][i];
    }

    // If parity check fails (parityBit is 1), we flip the corresponding data bit
    bitWrite(decodedData,i,parityBit);
  }

  return decodedData;
}

//Convolution codes
/*Test sketch:
void setup(){
  Serial.begin(9600);

  uint8_t testData = 0x55;  // Input byte (01010101)
  uint16_t encodedData = encodeConvolutional(testData);
  uint8_t decodedData = decodeConvolutional(encodedData);

  Serial.print("Input Data: "); Serial.println(testData, BIN);  // Print input in binary
  Serial.print("Encoded Data: "); Serial.println(encodedData, BIN);  // Print encoded output in binary
  Serial.print("Decoded Data: "); Serial.println(decodedData, BIN);
}

void loop(){
}*/

// Convolutional Encoder with G1 = 111 (7) and G2 = 101 (5) polynomials
uint16_t EspressoCM::encodeConvolutional(uint8_t inputByte) {
  uint8_t shiftReg = 0; // shift register to hold the state
  uint16_t encodedData = 0; // Holds the final encoded output for the entire byte

  // Loop through all 8 bits of the input byte
  for (int i=7; i >= 0; i--) {
    bool inputBit = bitRead(inputByte,i);//(inputByte >> i) & 0x01; // Get the current input bit (from MSB to LSB)
    shiftReg = (shiftReg << 1) | (inputBit & 0x01);
    bool output1 = bitRead(shiftReg,0) ^ bitRead(shiftReg,1) ^ bitRead(shiftReg,2);
    bool output2 = bitRead(shiftReg,0) ^ bitRead(shiftReg,2);
    //Serial.print(bitRead(shiftReg,2)); Serial.print(bitRead(shiftReg,1));Serial.print(bitRead(shiftReg,0)); Serial.print(" "); Serial.print(output1); Serial.println(output2);

    encodedData = (encodedData << 2) | (output1 << 1) | output2;
  }
  return encodedData; // Return the 16-bit encoded data for the entire byte
}

// Decoder function (Viterbi Algorithm)
uint8_t EspressoCM::decodeConvolutional(uint16_t encodedData) {
  static uint8_t shiftReg = 0; // Shift register to track previous states
  uint8_t decodedByte = 0;

  // Loop through the 16 bits of encoded data in pairs (2 bits per input bit)
  for(int i = 7; i >= 0; i--){
    bool output1 = bitRead(encodedData, (2 * i + 1));
    bool output2 = bitRead(encodedData, (2 * i));
    //Serial.print(output1); Serial.print(output2); Serial.print(" ");

    // Decide the most probable input bit based on output1 and output2
    bool inputBit;
    if(output1 == output2){
      inputBit = output1;
    }else{
      inputBit = shiftReg;
    }
    //Serial.print(inputBit); Serial.print(" ");

    // Update the shift register based on the current input bit
    shiftReg = (shiftReg << 1) | inputBit;
    //Serial.print(shiftReg, BIN); Serial.print(" ");

    decodedByte = (decodedByte << 1) | inputBit;
    //Serial.print(decodedByte, BIN); Serial.println(" ");
  }

  return decodedByte;
}
