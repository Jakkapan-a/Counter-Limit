#ifndef DATATYPEPROGRAM_H
#define DATATYPEPROGRAM_H


#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
// --------------------- Class --------------------- //
class DataTypeProgram {
public:
  int Lots;
  int LotSize;
  uint8_t IsBuzzer;
  float MinResistance;
  float MaxResistance;

  DataTypeProgram() {
    // Default values
    Lots = 0;
    LotSize = 0;
    IsBuzzer = 0;
    MinResistance = 9000;
    MaxResistance = 10000;
  }

  void readFromSD();

  void writeToSD();

  void updateValue(String key, String value);

private:
  void assignValue(String key, String value);
};

#endif