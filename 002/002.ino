#include <TcBUTTON.h>
#include <TcPINOUT.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#include <DS3231.h>
#include <EEPROM.h>

DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

// Sensor Pin D6
#define SensorPin 6
void SensorPressed(void);
TcBUTTON Sensor(SensorPin, SensorPressed, NULL);

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

// Alarm Pin D3
#define AlarmPin 7
void AlarmPIsOn(bool);
TcPINOUT Alarm(AlarmPin, AlarmPIsOn);

long total = 100;
long limit = 200;

// Time _time;
uint8_t date = 1;
uint8_t month = 1;
uint16_t year = 1;
uint16_t hour = 1;
uint16_t minute = 1;
uint16_t second = 1;

int myMenu[3] = { 0, 0, 0 };
int oldMenu[3] = { 0, 0, 0 };
// void updateDisplay(int);
const char* menuItems[] = { "DATE", "TIME", "COUNTER LIMIT", "RESET COUNTER", "ALARM" };
const int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);

const char* normalMenu[] = { "TOTAL", "TOTAL LIMIT", "LIMIT","DATE", "TIME"};
const int numNormalMenu = sizeof(normalMenu) / sizeof(normalMenu[0]);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Starting...");
  delay(250);  // wait for the OLED to power up
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  // display.begin(i2c_Address, true);  // Address 0x3C default
  //display.setContrast (0); // dim display
  updateDisplay();
  // display.clearDisplay();
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(0, 0);
  // display.println(" ");
  // // Print normalMenu and Add total and limit
  // for (int i = 0; i < numNormalMenu; i++) {
  //   if (i == 1) {
  //     display.setTextColor(BLACK, WHITE);
  //     display.print(" ");
  //     display.print(normalMenu[i]);
  //     display.println(" ");
  //   } else {
  //     display.setTextColor(WHITE);
  //     display.print(" ");
  //     display.print(normalMenu[i]);
  //     display.println(" ");
  //   }
  // }

  // display.display();
  
  delay(1);
  // updateDisplay(0);
  Serial.println("Started");
  // _mainMenuFunc();
  
}

void loop() {
  Esc.update();
  Up.update();
  Down.update();
  Enter.update();
  Sensor.update();


  // if (myMenu[0] != oldMenu[0] || myMenu[1] != oldMenu[1] || myMenu[2] != oldMenu[2]) {
  //   _mainMenuFunc();
  //   oldMenu[0] = myMenu[0];
  //   oldMenu[1] = myMenu[1];
  //   oldMenu[2] = myMenu[2];
  // }
}

void SensorPressed() {
  total++;
}

void AlarmPIsOn(bool state) {
  Serial.print("Alarm is ");
  Serial.println(state ? "On" : "Off");
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
}
void EscReleased() {
  Serial.println("Esc Released");
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
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 1) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    myMenu[2] = 2;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 2) {
    // 1 3 2
    myMenu[2] = 1;
  }

  if (myMenu[1] < 0) {
    myMenu[1] = 4;
  } else if (myMenu[1] > 4) {
    myMenu[1] = 0;
  }
  _mainMenuFunc();
}
void UpReleased() {
  Serial.println("Up Released");
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
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 1) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    myMenu[2] = 2;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 2) {
    // 1 3 2
    // RESET COUNTER MENU
    // No
    myMenu[2] = 1;
  }

  if (myMenu[1] < 0) {
    myMenu[1] = 4;
  } else if (myMenu[1] > 4) {
    myMenu[1] = 0;
  }

  _mainMenuFunc();
}
void DownReleased() {
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
}
void EnterReleased() {

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
             Serial.println("0 0 0");
             updateDisplay();
            // testdrawrect();
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
              Serial.println("1 0 0");
              // updateDisplay(0);
              testdrawrect();
              break;
          }
          break;
        case 1:
          switch (myMenu[2]) {
            case 0:
              // 1 1 0
              // TIME
              Serial.println("1 1 0");
              updateDisplay(1);
              break;
          }
          break;
        case 2:
          switch (myMenu[2]) {
            case 0:
              // 1 2 0
                Serial.println("1 2 0");
              updateDisplay(2);
              break;
          }
          break;
        case 3:
          switch (myMenu[2]) {
            case 0:
              // 1 3 0
              Serial.println("1 3 0");
              updateDisplay(3);
              break;
            case 1:
              // Reset Counter
              myMenu[2] = 2;
              break;
          }
          break;
        case 4:
          switch (myMenu[2]) {
            case 0:
              // 1 4 0
              Serial.println("1 4 0");
              updateDisplay(4);
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

void writeLong(int address, long value) {
  byte byte1 = (value & 0xFF);
  byte byte2 = ((value >> 8) & 0xFF);
  byte byte3 = ((value >> 16) & 0xFF);
  byte byte4 = ((value >> 24) & 0xFF);

  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
  EEPROM.update(address + 2, byte3);
  EEPROM.update(address + 3, byte4);
}

long readLong(int address) {
  long value = 0;
  value |= EEPROM.read(address);
  value |= ((long)EEPROM.read(address + 1) << 8);
  value |= ((long)EEPROM.read(address + 2) << 16);
  value |= ((long)EEPROM.read(address + 3) << 24);
  return value;
}

void updateDisplay(int index) {
  Serial.print("Index = ");
  Serial.println(index);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);  // 'inverted' text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(" ");
  // Print normalMenu and Add total and limit
  for (int i = 0; i < numMenuItems; i++) {
    if (i == index) {
      display.setTextColor(BLACK, WHITE);
      display.print(" >");
      display.print(menuItems[i]);
      display.println(" ");
      display.setTextColor(WHITE);
    } else {
    display.print(" ");
    display.print(menuItems[i]);
    display.println(" ");
    }    
  display.display();
    delay(10);
  }
  Serial.println("UPDATE OK");
}
void updateDisplay(void) {
  // Normal Menu
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);  // 'inverted' text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(" ");
  // Print normalMenu and Add total and limit
  for (int i = 0; i < numNormalMenu; i++) {
    display.print(" ");
    display.print(normalMenu[i]);
     display.println(" ");
    display.display();
      delay(10);
  }
  Serial.println("UPDATE NORMAL OK");
}

void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}