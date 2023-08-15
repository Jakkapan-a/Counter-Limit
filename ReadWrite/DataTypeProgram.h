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

  String FileName;
  String Measurement;
  String Min;
  String Max;
  String Date;
  String Time;

  DataTypeProgram() {
    // Default values
    Lots = 0;
    LotSize = 0;
    IsBuzzer = 1;
    MinResistance = 9000;
    MaxResistance = 10000;
  }

  void readFromSD();

  void writeToSD();

  void updateValue(String key, String value);

  void writeHistoryToSD();

private:
  void assignValue(String key, String value);
};

#endif