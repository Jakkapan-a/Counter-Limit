#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS 10
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK 13

DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;

LiquidCrystal_I2C lcd(0x3f, 16, 2);

// Sensor Pin D6
#define SensorPin 6
void SensorPressed(void);
void SensorReleased(void);
TcBUTTON Sensor(SensorPin, SensorPressed, SensorReleased);

// Button esc, up, down, enter = A0 A1 A2 A3
#define EscPin A0
#define UpPin A1
#define DownPin A2
#define EnterPin A3
void EscPressed();
void EscReleased();
void UpPressed();
void UpReleased();
void DownPressed();
void DownReleased();
void EnterPressed();
void EnterReleased();

TcBUTTON Esc(EscPin, EscPressed, EscReleased);
TcBUTTON Up(UpPin, UpPressed, UpReleased);
TcBUTTON Down(DownPin, DownPressed, DownReleased);
TcBUTTON Enter(EnterPin, EnterPressed, EnterReleased);

// Sensor Power pin 8
#define SensorPowerPin 8
void SensorPowerPressed();
void SensorPowerReleased();
TcBUTTON SensorPower(SensorPowerPin, SensorPowerPressed, SensorPowerReleased);


// Alarm Pin D3
#define AlarmPin 3
void AlarmPIsOn(bool);
TcPINOUT Alarm(AlarmPin, AlarmPIsOn);

// LCD Pin D7
#define LcdPin 7
void LcdPIsOn(bool);
TcPINOUT Lcd(LcdPin, LcdPIsOn);

unsigned long lastTime = 0;
unsigned long lastTime_ms = 0;
String filename = "Data.csv";

long total = 0;
long limit = 0;
long oldLimit = 0;
// Time _time;
uint8_t date = 0;
uint8_t month = 0;
uint16_t year = 0;
uint16_t hour = 0;
uint16_t minute = 0;
uint16_t second = 0;

int myMenu[3] = { 0, 0, 0 };
int oldMenu[3] = { 0, 0, 0 };

const char* menuItems[] = { "DATE", "TIME", "COUNTER LIMIT", "RESET COUNTER", "ALARM" };
const int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);

const char* normalMenu[] = { "TOTAL", "LIMIT", "TOTAL LIMIT", "DATE", "TIME" };
const int numNormalMenu = sizeof(normalMenu) / sizeof(normalMenu[0]);
bool isPressSoak = false;
int pressSoak = 0;
uint8_t isBtnSoak = 0;
uint8_t SleepLcd = 0;
const uint8_t SleepLcdMax = 30;
bool isOK = false;
int isAlarm = 0;
void setup() {
  Serial.begin(115200);
  Serial.println();
  Wire.begin();
  lcd.begin();
  SleepLcd = SleepLcdMax;
  Lcd.on();

   if (!SD.begin(SD_CS)) {
    Serial.println("SD card not found");
    lcd.setCursor(0, 0);
    lcd.print("SD card not found");
    while (1);
  }
  _mainMenuFunc();
  updateTime();
  filename = "D"+String(date)+"M"+String(month)+"Y"+String(year)+".csv";

  // Load total
  total = readLong(0);
  limit = readLong(20);
  isAlarm = readLong(30);
}

void loop() {
  Esc.update();
  Up.update();
  Down.update();
  Enter.update();
  Sensor.update();
  SensorPower.update();
  _timer();
  if (myMenu[0] != oldMenu[0] || myMenu[1] != oldMenu[1] || myMenu[2] != oldMenu[2]) {
    _mainMenuFunc();
    oldMenu[0] = myMenu[0];
    oldMenu[1] = myMenu[1];
    oldMenu[2] = myMenu[2];
  }
}
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
void updateTime() {
  year = myRTC.getYear();
  month = myRTC.getMonth(century);
  date = myRTC.getDate();

  hour = myRTC.getHour(h12Flag, pmFlag);
  minute = myRTC.getMinute();
  second = myRTC.getSecond();
}
void SensorPressed() {

}
void SensorReleased(){
  total++;
  _mainMenuFunc();
  saveLog();
}
void AlarmPIsOn(bool state) {
  Serial.print("Alarm is ");
  Serial.println(state ? "On" : "Off");
}
void LcdPIsOn(bool state) {
}

void EscPressed() {
  Serial.println("Esc Pressed");
  if (myMenu[0] != 0 && myMenu[1] == 0 && myMenu[2] == 0) {
    myMenu[0] = 0;
    myMenu[1] = 0;
    myMenu[2] = 0;
  } else if (myMenu[0] != 0 && myMenu[1] != -1 && myMenu[2] == 0) {
    myMenu[1] = 0;
  } else if (myMenu[0] != 0 && myMenu[1] != -1 && myMenu[2] != 0) {
    myMenu[2] = 0;
  }
  _mainMenuFunc();
  isPressSoak = true;
  isBtnSoak = 1;
  SleepLcd = SleepLcdMax;
}
void EscReleased() {
  Serial.println("Esc Released");
  isPressSoak = false;
  isBtnSoak = 0;
}
void UpPressed() {
  Serial.println("Up Pressed");
  if (myMenu[0] == 1 && myMenu[1] != -1 && myMenu[2] == 0) {
    myMenu[1] -= 1;
  }
  // 1 0 1 Show Date menu
  else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 2) {
    // 1 0 2
    // DATE MENU
    date += 1;
    if (date > 31 || date < 1) {
      date = 1;
    }
    // mainMenu();
  } else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 3) {
    // 1 0 3
    // DATE MENU
    month += 1;
    if (month > 12 || month < 1) {
      month = 1;
    }
    // mainMenu();
  } else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 4) {
    // 1 0 4
    // DATE MENU
    year += 1;
    if (year > 99 || year < 0) {
      year = 0;
    }
    // mainMenu();
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 0) {
    // 1 1 0
    // TIME MENU
    myMenu[1] -= 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 2) {
    // 1 1 2
    // TIME MENU
    hour += 1;
    if (hour > 23 || hour < 0) {
      hour = 0;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 3) {
    // 1 1 3
    // TIME MENU
    minute += 1;
    if (minute > 59 || minute < 0) {
      minute = 0;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 4) {
    // 1 1 4
    // TIME MENU
    second += 1;
    if (second > 59 || second < 0) {
      second = 0;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 2 && myMenu[2] == 0) {
    // 1 2 0
    // COUNTER LIMIT MENU
    myMenu[1] -= 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 2 && myMenu[2] == 2) {
    // 1 2 2
    // COUNTER LIMIT MENU
    limit += 1;
    if (limit > 999999 || limit < 0) {
      limit = 0;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 0) {
    // 1 3 0
    // RESET COUNTER MENU
    myMenu[1] -= 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 2) {
    // 1 3 2
    // RESET COUNTER MENU
    // Yes
    isOK = !isOK;
  } else if (myMenu[0] == 1 && myMenu[1] == 4 && myMenu[2] == 0) {
    // 1 4 0
    // ALARM MENU
    myMenu[1] -= 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 4 && myMenu[2] == 2) {
    // 1 4 2
    // ALARM MENU
    // Yes
    isOK = !isOK;
  }

  if (myMenu[1] < 0) {
    myMenu[1] = 4;
  } else if (myMenu[1] > 4) {
    myMenu[1] = 0;
  }
  _mainMenuFunc();
  isPressSoak = true;
  isBtnSoak = 2;
  SleepLcd = SleepLcdMax;
}
void UpReleased() {
  Serial.println("Up Released");
  isPressSoak = false;
  isBtnSoak = 0;
}
void DownPressed() {
  Serial.println("Down Released");
  Serial.println("Down Released");
  if (myMenu[0] == 1 && myMenu[1] != -1 && myMenu[2] == 0) {
    myMenu[1] += 1;
  }
  // 1 0 1 Show Date menu
  else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 2) {
    // 1 0 2
    // DATE MENU
    date -= 1;
    if (date > 31 || date < 1) {
      date = 31;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 3) {
    // 1 0 3
    // DATE MENU
    month -= 1;
    if (month > 12 || month < 1) {
      month = 12;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 0 && myMenu[2] == 4) {
    // 1 0 4
    // DATE MENU
    year -= 1;
    if (year > 99 || year < 0) {
      year = 99;
    }

  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 0) {
    // 1 1 0
    // TIME MENU
    myMenu[1] += 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 2) {
    // 1 1 2
    // TIME MENU
    hour -= 1;
    if (hour > 23 || hour < 0) {
      hour = 23;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 3) {
    // 1 1 3
    // TIME MENU
    minute -= 1;
    if (minute > 59 || minute < 0) {
      minute = 59;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 1 && myMenu[2] == 4) {
    // 1 1 4
    // TIME MENU
    second -= 1;
    if (second > 59 || second < 0) {
      second = 59;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 2 && myMenu[2] == 0) {
    // 1 2 0
    // COUNTER LIMIT MENU
    myMenu[1] += 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 2 && myMenu[2] == 2) {
    // 1 2 2
    // COUNTER LIMIT MENU
    limit -= 1;
    if (limit > 999999 || limit < 0) {
      limit = 999999;
    }
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 0) {
    // 1 3 0
    // RESET COUNTER MENU
    myMenu[1] += 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 2) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    isOK = !isOK;
  } else if (myMenu[0] == 1 && myMenu[1] == 4 && myMenu[2] == 0) {
    // 1 4 0
    // ALARM MENU
    myMenu[1] += 1;
  } else if (myMenu[0] == 1 && myMenu[1] == 4 && myMenu[2] == 2) {
    // 1 4 1
    // ALARM MENU
    // Yes
    isOK = !isOK;
  }

  if (myMenu[1] < 0) {
    myMenu[1] = 4;
  } else if (myMenu[1] > 4) {
    myMenu[1] = 0;
  }
  _mainMenuFunc();
  isPressSoak = true;
  isBtnSoak = 3;
  SleepLcd = SleepLcdMax;
}
void DownReleased() {
  Serial.println("Down Released");
  isPressSoak = false;
  isBtnSoak = 0;
}
void EnterPressed() {
  Serial.println("Enter Pressed");
  if (myMenu[0] == 0 && myMenu[1] == 0 && myMenu[2] == 0) {
    myMenu[0] = 1;
    myMenu[1] = 0;
    myMenu[2] = 0;
  } else {
    if (myMenu[1] >= 0 && myMenu[0] == 1) {
      myMenu[2] += 1;
      if (myMenu[2] > 9) {
        myMenu[2] = 0;
      }
    } else {
      myMenu[1] = 0;
    }
  }
  _mainMenuFunc();
  isPressSoak = true;
  isBtnSoak = 4;
  SleepLcd = SleepLcdMax;
}
void EnterReleased() {
  Serial.println("Enter Released");
  isPressSoak = false;
  isBtnSoak = 0;
}

void _mainMenuFunc() {

  Serial.print("Menu Code = ");
  Serial.print(myMenu[0]);
  Serial.print(", ");
  Serial.print(myMenu[1]);
  Serial.print(", ");
  Serial.println(myMenu[2]);
  switch (myMenu[0]) {
    case 0:
      // Normal
      // 0 x x
      switch (myMenu[1]) {
        case 0:
          switch (myMenu[2]) {
            case 0:
              // lcd.backlight();
              updateLCD();

              break;
          }
          break;
      }

      break;
    case 1:
      // Setting Menu
      // 1 x x
      switch (myMenu[1]) {
        case 0:
          switch (myMenu[2]) {
            case 0:
              // Date
              // 1 0 0
              updateLCD(0);
              break;
            case 1:
              // 1 0 1
              // get date from RTC
              date = myRTC.getDate();
              month = myRTC.getMonth(century);
              year = myRTC.getYear();
              myMenu[2] = 2;
              break;
            case 2:
              // 1 0 2
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("DATE = ");
              lcd.print(date);
              lcd.blink();
              break;
            case 3:
              // 1 0 3
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("MONTH = ");
              lcd.print(month);
              lcd.blink();
              break;
            case 4:
              // 1 0 4
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("YEAR = ");
              lcd.print(year);
              lcd.blink();
              break;
            case 5:
              // 1 0 5
              // set date to RTC
              myRTC.setDate(date);
              myRTC.setMonth(month);
              myRTC.setYear(year);
              myMenu[2] = 0;
              lcd.noBlink();
              break;
          }
          break;
        case 1:
          switch (myMenu[2]) {
            case 0:
              // 1 1 0
              updateLCD(1);
              break;
            case 1:
              // 1 1 1
              // get time from RTC
              hour = myRTC.getHour(h12Flag, pmFlag);
              minute = myRTC.getMinute();
              second = myRTC.getSecond();
              myMenu[2] = 2;
              break;
            case 2:
              // 1 1 2
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("HOUR = ");
              lcd.print(hour);
              lcd.blink();
              break;
            case 3:
              // 1 1 3
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("MINUTE = ");
              lcd.print(minute);
              lcd.blink();
              break;
            case 4:
              // 1 1 4
              lcd.setCursor(0, 0);
              lcd.clear();
              lcd.print("SECOND = ");
              lcd.print(second);
              lcd.blink();
              break;
            case 5:
              // 1 1 5
              // set time to RTC
              myRTC.setHour(hour);
              myRTC.setMinute(minute);
              myRTC.setSecond(second);
              myMenu[2] = 0;
              lcd.noBlink();
              break;
          }
          break;
        case 2:
          switch (myMenu[2]) {
            case 0:
              // 1 2 0
              updateLCD(2);
              break;
            case 1:
              // 1 2 1
              lcd.clear();
              myMenu[2] = 2;
              break;
            case 2:
              // 1 2 2
              oldLimit > limit ? lcd.clear() : lcd.blink();
              oldLimit = limit;
              lcd.setCursor(0, 0);
              lcd.print("LIMIT = ");
              lcd.print(limit);
              lcd.blink();
              break;
            case 3:
              // 1 2
              // Save limit to EEPROM
              writeLong(20, limit);
              myMenu[2] = 0;
              break;
          }
          break;
        case 3:
          switch (myMenu[2]) {
            case 0:
              // Rest counter
              // 1 3 0
              updateLCD(3);
              break;
            case 1:
              // 1 3 1
              myMenu[2] = 2;
              break;
            case 2:
              // 1 3 2
              if (isOK) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(">");
                lcd.print("YES");
                lcd.setCursor(0, 1);
                lcd.print("NO");
              } else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("YES");
                lcd.setCursor(0, 1);
                lcd.print(">");
                lcd.print("NO");
              }
              break;
            case 3:
              // 1 4 3
              if (isOK) {
                total = 0;
                writeLong(0, total);
              }
              myMenu[2] = 0;
              break;
          }
          break;
        case 4:
          switch (myMenu[2]) {
            case 0:
              // 1 4 0
              updateLCD(4);
              break;
            case 1:
              // 1 4 1
              lcd.clear();
              myMenu[2] = 2;
              break;
            case 2:
              // 1 4 2
              if (isOK) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(">");
                lcd.print("ON");
                lcd.setCursor(0, 1);
                lcd.print("OFF");
              } else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("ON");
                lcd.setCursor(0, 1);
                lcd.print(">");
                lcd.print("OFF");
              }
              break;
            case 3:
              // 1 4 3
              if (isOK) {
                writeLong(30, 1);
              } else {
                writeLong(30, 0);
              }
              myMenu[2] = 0;
              break;
          }
          break;
        default:
          Serial.println("Error");
          if (myMenu[1] > 4) {
            myMenu[1] = 0;
          } else if (myMenu[1] < 0) {
            myMenu[1] = 4;
          }
          break;
      }
      break;
    default:
      Serial.println("Error");
      if (myMenu[0] > 1) {
        myMenu[0] = 0;
      } else if (myMenu[1] < 0) {
        myMenu[0] = 4;
      }
      _mainMenuFunc();
      break;
  }
}

int getMenuCode() {
  char temp[10];
  char _menuCode[10] = "";
  for (int i = 0; i < 3; i++) {
    itoa(myMenu[i], temp, 10);  // แปลงค่า int เป็น char array
    strcat(_menuCode, temp);    // ต่อ char array
  }
  return atoi(_menuCode);
}

long oldTotal = 0;
int oldSecond = 0;
void updateLCD() {
  // if (oldTotal > total) { lcd.clear(); }
  // if (oldSecond > second) { lcd.clear(); }
  lcd.clear();
  oldTotal = total;
  lcd.noBlink();
  lcd.print("TOTAL= ");
  lcd.print(total);
  lcd.print("/");
  lcd.print(limit);
  lcd.setCursor(0, 1);
  // get date from RTC
  updateTime();
  oldSecond = second;
  lcd.print(date);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  lcd.print(" ");
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(second);
}
void updateLCD(uint8_t index) {
  lcd.clear();
  lcd.noBlink();
  lcd.print(">");
  lcd.print(menuItems[index]);
  lcd.setCursor(0, 1);
  lcd.print(index + 1 > 4 ? " " : menuItems[index + 1]);
}
void _timer() {
  if (millis() - lastTime > 1000) {
    if (isPressSoak) {
      pressSoak++;
    } else {
      pressSoak = 0;
    }
    if (SleepLcd > 0) {
      SleepLcd--;
      if (SleepLcd == 1) {
        myMenu[0] = 0;
        myMenu[1] = 0;
        myMenu[2] = 0;
        oldMenu[0] = 0;
        oldMenu[1] = 0;
        oldMenu[2] = 0;
      }
    }
  
    if (myMenu[0] == 0 && myMenu[1] == 0 && myMenu[2] == 0) {
        if(total >= limit){
      if(isAlarm == 1 && !Alarm.isOn()){
        Alarm.on(); 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALARM");
        lcd.setCursor(0, 1);
        lcd.print("TOTAL= ");
        lcd.print(total);
        lcd.print("/");
        lcd.print(limit);
      }else{
        Alarm.off();
        Serial.print("Alarm= ");
        Serial.println(isAlarm);
      }
    }else{
      if(Alarm.isOn()){
        Alarm.off();
      }
      updateLCD();
    }
    }
    lastTime = millis();
  } else if (millis() < 1000) {
    lastTime = millis();
  }

  if (SleepLcd == 1) {
    Lcd.off();
  } else if (SleepLcd > 1) {
    Lcd.on();
  }
  //
  if (pressSoak > 5) {
    switch (isBtnSoak) {
      case 1:
        EscPressed();
        break;
      case 2:
        UpPressed();
        break;
      case 3:
        DownPressed();
        break;
      case 4:
        EnterPressed();
        break;
    }
  }
}

void saveLog(){
  filename = "Log"+String(year)+String(month)+String(date)+".csv";

  if (!SD.exists(filename)) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.println("DateTime,Total,Limit");
      file.close();
    }
  }

  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.print(date);
    file.print("/");
    file.print(month);
    file.print("/");
    file.print(year);
    file.print(" ");
    file.print(hour);
    file.print(":");
    file.print(minute);
    file.print(":");
    file.print(second);
    file.print(",");
    file.print(total);
    file.print(",");
    file.println(limit);
    file.close();
  }
}
void saveLog(String msg){
  filename = "Log"+String(year)+String(month)+String(date)+".csv";
  if (!SD.exists(filename)) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.println("DateTime,Total,Limit");
      file.close();
    }else{
      Serial.println("File 1 not found");
      lcd.clear();
      lcd.print("File 1 not found");
      return;
    }
  }
  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.print(date);
    file.print("/");
    file.print(month);
    file.print("/");
    file.print(year);
    file.print(" ");
    file.print(hour);
    file.print(":");
    file.print(minute);
    file.print(":");
    file.print(second);
    file.print(",");
    file.print(total);
    file.print(",");
    file.print(limit);
    file.print(",");
    file.println(msg);
    file.close();
  }else {
    Serial.println("File 2 not found");
    lcd.clear();
    lcd.print("File 2 not found");
    return;
  }
}


void SensorPowerPressed()
{

}

void SensorPowerReleased(){
  // Save total
  writeLong(0, total);

  // Save log to SD
  saveLog("Power off");
  Serial.println("Power off");
}