/*
Name:     Espresso.h
Created:  10/11/2022 2:27:23 PM
Author:   Brandon
*/

#ifndef Espresso_h
#define Espresso_h

#include "Arduino.h"

#define CORE1 1
#define CORE2 2

//#define SS 10
//#define MOSI 11
//#define SCK 12
//#define MISO 13
#define E2B_EXTERNAL 21
#define VREF_VBATT_pin 4
#define CORESEL0 37
#define CORESEL1 36

class EspressoCM{
  // user-accessible "public" interface
  public:
    EspressoCM();
    void setOffset_VBATT(float offset);
    void setOffset_ISNS(float offset);
    bool getPowerSource();
    float getBatteryVoltage();
    float getBatteryPercentage();
    float getBoardCurrent();
    //String generate_password();
    //void establish_WiFi_server();

    //String username;

    uint8_t CORE;

  // library-accessible "private" interface
  private:
    //String password;

    float offset_VBATT;
    float offset_ISNS;
};

#endif
