/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>
// --------------------- Class --------------------- //
class DataTypeProgram {
public:
  int Lots;
  int LotSize;
  bool IsBuzzer;
  float MinResistance;
  float MaxResistance;

  DataTypeProgram() {
    // Default values
    Lots = 0;
    LotSize = 0;
    IsBuzzer = false;
    MinResistance = 9000;
    MaxResistance = 10000;
  }

  void readFromSD() {
    File dataFile = SD.open("settings.txt", FILE_READ);
    if (dataFile) {
      while (dataFile.available()) {
        String line = dataFile.readStringUntil('\n');
        Serial.println(line);
        int separatorIndex = line.indexOf('=');
        String key = line.substring(0, separatorIndex);
        String value = line.substring(separatorIndex + 1);
        assignValue(key, value);
      }
      dataFile.close();
    } else {
      dataFile.close();
      // If file doesn't exist, create a new one with default values
      writeToSD();
    }
  }

  void writeToSD() {
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

  void updateValue(String key, String value) {
    assignValue(key, value);
    writeToSD();
  }

private:
  void assignValue(String key, String value) {
    if (key == "Lots") {
      Lots = value.toInt();
    } else if (key == "LotSize") {
      LotSize = value.toInt();
    } else if (key == "IsBuzzer") {
      IsBuzzer = value == "true";
    } else if (key == "MinResistance") {
      MinResistance = value.toFloat();
    } else if (key == "MaxResistance") {
      MaxResistance = value.toFloat();
    }
  }
};
// -------------------- Variable -------------------- //
DataTypeProgram dataProgram;
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
 

   dataProgram.readFromSD();

  // Example of updating a value
  // dataProgram.updateValue("Lots", "15");

  Serial.println("Lots: " + String(dataProgram.Lots));

}

void loop() {
  // nothing happens after setup
}


