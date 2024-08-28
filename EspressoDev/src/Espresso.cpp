/*
Name:     Espresso.cpp
Created:  8/8/24 8:12:06 PM
Author:   Brandon
*/

#include "Arduino.h"
#include "Espresso.h"



EspressoCM::EspressoCM(){
  #if defined(ARDUINO_ARCH_ESP32)
    #define LED_BUILTIN 46
    #define E2B_BUILTIN 9
    #define ISNS_pin 5
    if(( digitalRead(CORESEL0) ) && ( !digitalRead(CORESEL1) )){
      uint8_t CORE = 1;
    }
    if(( !digitalRead(CORESEL0) ) && ( digitalRead(CORESEL1) )){
      uint8_t CORE = 2;
    }
  #elif defined(__AVR_ATtiny85__)
    #define LED_BUILTIN 3
    #define E2B_BUILTIN 2
    #define ISNS_pin 4
    uint8_t CORE = 3;
  #endif

  //String username = "";
  //String password = "";

  float offset_VBATT = 0;
  float offset_ISNS = 0;

}

//Sets the offset for the voltage sensing line.
void EspressoCM::setOffset_VBATT(float offset){
  float offset_VBATT = offset;
}

//Sets the offset for the current sensing line.
void EspressoCM::setOffset_ISNS(float offset){
  float offset_ISNS = offset;
}

//Returns the battery voltage.
float EspressoCM::getBatteryVoltage(){
  int value = analogRead(VREF_VBATT_pin);
  float voltage = value * 3.3/1023;
  voltage + offset_VBATT;

  return voltage;
}

//Returns which supply is powering the CM
// Returns 1 if powered via battery, returns 0 if powered by main power connector(s)
bool EspressoCM::getPowerSource(){
  if(analogRead(VREF_VBATT_pin) > 10){
    return 1;
  }else{
    return 0;
  }
}

//Returns the battery percentage.
float EspressoCM::getBatteryPercentage(){
  int value = analogRead(VREF_VBATT_pin);
  float voltage = value * 3.3/1023;
  voltage + offset_VBATT;

  if ((voltage >= 3.7) && (voltage <= 4.2)){
    vbat = 3.7;
  }else if ((voltage > 4.2) && (voltage <= 4.76)){
    vbat = 4.2;
  }/*else{
    vbat = 3.7;     //Used as a catch-all. Espresso is built for use with 3.7V batteries so 3.7V is default.
  }*/

  float perc = map(voltage, vbat, vbat*1.135, 0, 100);

  return perc;
}

//Returns the current draw of the Espresso CM.
float EspressoCM::getBoardCurrent(){
  int value = analogRead(ISNS_pin);
  float voltage = value * 3.3/1023;
  voltage + offset_ISNS;

  return voltage;
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
