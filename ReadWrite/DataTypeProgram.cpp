#include "DataTypeProgram.h"

void DataTypeProgram::readFromSD() {
  File dataFile = SD.open("settings.txt", FILE_READ);
  if (dataFile) {
    while (dataFile.available()) {
      String line = dataFile.readStringUntil('\n');
      int separatorIndex = line.indexOf('=');
      String key = line.substring(0, separatorIndex);
      String value = line.substring(separatorIndex + 1);
      assignValue(key, value);
    }
    dataFile.close();
  } else {
    // If file doesn't exist, create a new one with default values
    writeToSD();
  }
}

void DataTypeProgram::writeToSD() {
  // First, remove the old file
  // SD.remove("settings.txt");

  File dataFile = SD.open("settings.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Lots=" + String(Lots));
    dataFile.println("LotSize=" + String(LotSize));
    dataFile.println("IsBuzzer=" + String(IsBuzzer));
    dataFile.println("MinResistance=" + String(MinResistance, 2));  // with 2 decimal places
    dataFile.println("MaxResistance=" + String(MaxResistance, 2));

    dataFile.close();
  }
}

void DataTypeProgram::updateValue(String key, String value) {
  assignValue(key, value);
  writeToSD();
}

void DataTypeProgram::assignValue(String key, String value) {
  if (key == "Lots") {
    Lots = value.toInt();
  } else if (key == "LotSize") {
    LotSize = value.toInt();
  } else if (key == "IsBuzzer") {
    IsBuzzer = value.toInt();
  } else if (key == "MinResistance") {
    MinResistance = value.toFloat();
  } else if (key == "MaxResistance") {
    MaxResistance = value.toFloat();
  }
}


void DataTypeProgram::writeHistoryToSD() {
  String fileName = this->FileName;
  File dataFile = SD.open(fileName + ".csv", FILE_WRITE);
  if (dataFile) {
    // Write the CSV header if the file is new (size is 0)
    if (dataFile.size() == 0) {
      dataFile.println("Measure(Ohm),Min(Ohm),Max(Ohm),Date,Time");
    }

    // Append new data
    dataFile.println(Measurement + "," + Min + "," + Max + "," + Date + "," + Time);

    // Close the file
    dataFile.close();
  } else {
    Serial.println("Error opening file");
  }
}