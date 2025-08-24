#ifndef Espresso_h
#define Espresso_h

#include <Arduino.h>

#define vbat 3.7
#define CORE1 1
#define CORE2 2

class EspressoCM{
  // user-accessible "public" interface
  public:
    EspressoCM();
    uint8_t getCoreNumber();
    uint8_t runDiagnostics();

    float getBoardLDOEfficiency(float _pin, float _pout);
    float getBoardHeatDissipation(float _pin, float _pout);
    float getRegulatorTemperatureRise(float _pin, float _pout);
    float getRequiredHeatsinkThermalResistance(float _pin, float _pout,float Tmax, float Tambient, float RthJC, float RthCS);
    float getRequiredAirflow(float _pin, float _pout, float allowedRise);

    float mils(float val_mm);
    float millimeters(float val_mils);
    static uint8_t checksum(const uint8_t *addr, uint8_t len);
    uint8_t hammingEncode(uint8_t data);
    uint8_t hammingDecode(uint8_t encodedData);
    uint16_t encodeLDPC(byte data);
    byte decodeLDPC(uint16_t encodedData);
    uint16_t encodeConvolution(uint8_t inputByte);
    uint8_t decodeConvolution(uint16_t encodedData);
    //String generate_password();
    //void establish_WiFi_server();

    uint8_t CORE;
    uint8_t CORESEL0;
    uint8_t CORESEL1;
    uint8_t LEDpin;
    uint8_t E2B_BUILTIN;
    uint8_t E2B_EXTERNAL;
    uint8_t CS_FLASH;
    //String username;

  // library-accessible "private" interface
  private:
    //String password;
};

#endif
