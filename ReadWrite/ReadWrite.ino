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
#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>
#include <EEPROM.h>

#include "DataTypeProgram.h"

#define SD_CS 10
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK 13

#define BUTTON_ESC_PIN 4
#define BUTTON_UP_PIN 6
#define BUTTON_DOWN_PIN 7
#define BUTTON_ENTER_PIN 5
#define BUZZER_PIN 9

// --------------------- Class --------------------- //
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
  dataProgram.updateValue("Lots", "15");

  Serial.println("Lots: " + String(dataProgram.Lots));

}

void loop() {
  // nothing happens after setup
}


