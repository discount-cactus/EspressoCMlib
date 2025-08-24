//Convolution code Test
/*NOTES:
-This code demonstrates convolution code encoding and decoding. You can modify it to suit your specific requirements.
-This implementation supports single-bit and multiple-bit errors
*/
#include "Espresso.h"

EspressoCM cm;

void setup(){
  Serial.begin(9600);
  while(!Serial){}

  uint8_t testData = 0x55;  // Input byte (01010101)
  uint16_t encodedData = cm.encodeConvolution(testData);
  uint8_t decodedData = cm.decodeConvolution(encodedData);

  Serial.print("Input Data: "); Serial.println(testData, BIN);  // Print input in binary
  Serial.print("Encoded Data: "); Serial.println(encodedData, BIN);  // Print encoded output in binary
  Serial.print("Decoded Data: "); Serial.println(decodedData, BIN);
}

void loop(){
}