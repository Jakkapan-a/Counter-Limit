#include <EEPROM.h>


void writeLong(uint32_t address, uint32_t value) {
  byte byte1 = (value & 0xFF);
  byte byte2 = ((value >> 8) & 0xFF);
  byte byte3 = ((value >> 16) & 0xFF);
  byte byte4 = ((value >> 24) & 0xFF);

  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
  EEPROM.update(address + 2, byte3);
  EEPROM.update(address + 3, byte4);
}

uint32_t readLong(uint32_t address) {
  uint32_t value = 0;
  value |= EEPROM.read(address);
  value |= ((uint32_t)EEPROM.read(address + 1) << 8);
  value |= ((uint32_t)EEPROM.read(address + 2) << 16);
  value |= ((uint32_t)EEPROM.read(address + 3) << 24);
  return value;
}

void setup() {
  Serial.begin(9600);

  // ตัวอย่างการเขียนค่าลง EEPROM
  uint32_t address = 0;
  uint32_t valueToWrite = 123456789;
  writeLong(address, valueToWrite);
  Serial.print("เขียนค่า ");
  Serial.print(valueToWrite);
  Serial.println(" ลงใน EEPROM ที่ตำแหน่ง ");
  Serial.println(address);
  
  delay(500);

  // ตัวอย่างการอ่านค่าจาก EEPROM
  uint32_t readValue = readLong(address);
  Serial.print("อ่านค่าจาก EEPROM ที่ตำแหน่ง ");
  Serial.print(address);
  Serial.print(" ได้ค่า ");
  Serial.println(readValue);
}

void loop() {
  // put your main code here, to run repeatedly:

}
