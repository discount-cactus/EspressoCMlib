//Hamming Code Test
/*NOTES:
-This code demonstrates Hamming(7,4) encoding and decoding. You can modify it to suit your specific requirements.
-This implementation assumes a single-bit error. For multiple-bit errors, consider using more advanced error-correcting codes.
*/
#include "Espresso.h"

EspressoCM cm;

void setup() {
  Serial.begin(9600);
  while(!Serial){}

  uint8_t data = 0xA; // 1010 in binary
  uint8_t encodedData = cm.hammingEncode(data);
  uint8_t decodedData = cm.hammingDecode(encodedData);

  Serial.print("Input Data: "); Serial.println(data,HEX);
  Serial.print("Encoded Data: "); Serial.println(encodedData, BIN);
  Serial.print("Decoded Data: "); Serial.println(decodedData,HEX);
}

void loop(){
}