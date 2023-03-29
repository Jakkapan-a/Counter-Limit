#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <DS3231.h>
#include <EEPROM.h>

DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sensor Pin D6
#define SensorPin 6
void SensorPressed(void);
TcBUTTON Sensor(SensorPin, SensorPressed, NULL);

// Button esc, up, down, enter = A0 A1 A2 A3
#define EscPin A0
#define UpPin A2
#define DownPin A1
#define EnterPin A3
void EscPressed(void);
void EscReleased(void);
void UpPressed(void);
void UpReleased(void);
void DownPressed(void);
void DownReleased(void);
void EnterPressed(void);
void EnterReleased(void);

TcBUTTON Esc(EscPin, EscPressed, EscReleased);
TcBUTTON Up(UpPin, UpPressed, UpReleased);
TcBUTTON Down(DownPin, DownPressed, DownReleased);
TcBUTTON Enter(EnterPin, EnterPressed, EnterReleased);

// Alarm Pin D3
#define AlarmPin 3
void AlarmPIsOn(bool);
TcPINOUT Alarm(AlarmPin, AlarmPIsOn);

String msg = "";
unsigned long lastTime = 0;
unsigned long lastTime_ms = 0;

long total = 0;
long limit = 0;

// Time _time;
uint8_t date = 0;
uint8_t month = 0;
uint16_t year = 0;
uint16_t hour = 0;
uint16_t minute = 0;
uint16_t second = 0;


int menu[3] = { 0, 0, 0 };
int oldMenu[3] = { 0, 0, 0 };

void _mainMenuFunc(void);
void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Wire.begin();


  display.begin(i2c_Address);  // Address 0x3C default
  // SensorPressed();
  display.clearDisplay();
  display.display();
  //  menu[1] = 5;
  Serial.println("Started");
  // _mainMenuFunc() ;
  // test();
}

void loop() {
  Sensor.update();
  Esc.update();
  Up.update();
  Down.update();
  Enter.update();
  _timer();

  if (menu[0] != oldMenu[0] || menu[1] != oldMenu[1] || menu[2] != oldMenu[2]) {
    // Update the menu
 
    // Serial.println("ffffffffffffffffffffffffffffffffff");
    // Serial.print("Menu[ = ");
    // Serial.print(menu[0]);
    // Serial.print(", ");
    // Serial.print(menu[1]);
    // Serial.print(", ");
    // Serial.println(menu[2]);

    oldMenu[0] = menu[0];
    oldMenu[1] = menu[1];
    oldMenu[2] = menu[2];
    _mainMenuFunc();
  }
  

}

void _mainMenuFunc() {

Serial.print("ts");

  switch (menu[0]) {
    case 0:
      // Normal 
      break;
    case 1:
      // 1 x x
      switch (menu[1]) {
        case 0 :
        // 1 0 x
        switch(menu[2]){
          case 0:
          // 1 0 0
          // display.clearDisplay();
          // display.setTextSize(1);
          // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
          // display.setTextColor(SH110X_WHITE);
          // display.setCursor(0, 0);
          // display.println("TESt");
          // display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          // display.println("DATE");
          // display.setTextColor(SH110X_WHITE);
          // display.println("TIME");
          // display.println("COUNTER LIMIT");
          // display.println("RESET COUNTER");
          // display.println("ALARM");
          // display.display();
          break;
          case 1:
          // 1 0 1
          // DateTime now = myRTC.now();
          // date = myRTC.getDate();
          //  month = myRTC.getMonth(century);
          // year = myRTC.getYear();

          // Serial.print("Date = ");
          // Serial.print(date);
          // Serial.print(" ");
          // Serial.print(month);
          // Serial.print(" ");
          // Serial.println(year);

          
          menu[2] = 2;
          break;
          case 2:
          // 1 0 2
          // DATE 
          // display.clearDisplay();
          // display.setTextSize(1);
          // // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
          // display.setTextColor(SH110X_WHITE);
          // display.setCursor(0, 0);
          // display.println(" ");       
          // display.print("DATE = ");
          // display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          // display.print(date,DEC);
          // display.setTextColor(SH110X_WHITE);
          // display.print(" ");
          // display.print(month,DEC);
          // display.print(" ");
          // display.println(year,DEC);
          // display.display();
          break;
          case 3:
          // 1 0 3
          // MONTH
          // display.clearDisplay();
          // display.setTextSize(1);
          // // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
          // display.setTextColor(SH110X_WHITE);
          // display.setCursor(0, 0);
          // display.println("");       
          // display.print("DATE = ");
          // display.print(date);
          // display.print(" ");
          // display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          // display.print(month);
          // display.setTextColor(SH110X_WHITE);
          // display.print(" ");
          // display.println(year);
          // display.display();
          break;
          case 4:
          // 1 0 4
          // YEAR
          // display.clearDisplay();
          // display.setTextSize(1);
          // // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
          // display.setTextColor(SH110X_WHITE);
          // display.setCursor(0, 0);
          // display.println("");
          // display.print("DATE = ");
          // display.print(date);
          // display.print(" ");
          // display.print(month);
          // display.print(" ");
          // display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          // display.println(year);
          // display.display();
          break;
        }
        break;
      }

      break;
  }

}

void _timer() {
  if (millis() - lastTime > 1000) {
    lastTime = millis();
  } else if (millis() < 1000) {
    lastTime = millis();
  }
}
void SensorPressed() {
  total++;
}

void AlarmPIsOn(bool state) {
  Serial.print("Alarm is ");
  Serial.println(state ? "On" : "Off");
}

void EscPressed() {

  // Serial.println("Esc Pressed");
  if (menu[0] != 0 && menu[1] != -1 && menu[2] == 0) {
    menu[0] = 0;
  } else if (menu[0] != 0 && menu[1] != -1 && menu[2] == 0) {
    menu[1] = 0;
  } else if (menu[0] != 0 && menu[1] != -1 && menu[2] != 0) {
    menu[2] = 0;
  }

  // Serial.print("==============================");
}
void EscReleased() {
  // Serial.println("Esc Released");
}
void UpPressed() {

  // Serial.println("Up Pressed");
  if (menu[0] == 1 && menu[1] != -1 && menu[2] == 0) {
    menu[1]++;
  }
  // 1 0 1 Show Date menu
  else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 2) {
    // 1 0 2
    // DATE MENU
    date += 1;
    if (date > 31 || date < 1) {
      date = 1;
    }
    // mainMenu();
  } else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 3) {
    // 1 0 3
    // DATE MENU
    month += 1;
    if (month > 12 || month < 1) {
      month = 1;
    }
    // mainMenu();
  } else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 4) {
    // 1 0 4
    // DATE MENU
    year += 1;
    if (year > 99 || year < 0) {
      year = 0;
    }
    // mainMenu();
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 0) {
    // 1 1 0
    // TIME MENU
    menu[1]++;
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 2) {
    // 1 1 2
    // TIME MENU
    hour += 1;
    if (hour > 23 || hour < 0) {
      hour = 0;
    }
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 3) {
    // 1 1 3
    // TIME MENU
    minute += 1;
    if (minute > 59 || minute < 0) {
      minute = 0;
    }
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 4) {
    // 1 1 4
    // TIME MENU
    second += 1;
    if (second > 59 || second < 0) {
      second = 0;
    }
  } else if (menu[0] == 1 && menu[1] == 2 && menu[2] == 0) {
    // 1 2 0
    // COUNTER LIMIT MENU
    menu[1]++;
  } else if (menu[0] == 1 && menu[1] == 2 && menu[2] == 2) {
    // 1 2 2
    // COUNTER LIMIT MENU
    limit += 1;
    if (limit > 999999 || limit < 0) {
      limit = 0;
    }
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 0) {
    // 1 3 0
    // RESET COUNTER MENU
    menu[1]++;
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 1) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    menu[2] = 2;
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 2) {
    // 1 3 2
    menu[2] = 1;
  }
}
void UpReleased() {
}
void DownPressed() {
  Serial.println("Down Released");
  if (menu[0] == 1 && menu[1] != -1 && menu[2] == 0) {
    menu[1]--;
  }
  // 1 0 1 Show Date menu
  else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 2) {
    // 1 0 2
    // DATE MENU
    date -= 1;
    if (date > 31 || date < 1) {
      date = 31;
    }
  } else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 3) {
    // 1 0 3
    // DATE MENU
    month -= 1;
    if (month > 12 || month < 1) {
      month = 12;
    }
  } else if (menu[0] == 1 && menu[1] == 0 && menu[2] == 4) {
    // 1 0 4
    // DATE MENU
    year -= 1;
    if (year > 99 || year < 0) {
      year = 99;
    }

  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 0) {
    // 1 1 0
    // TIME MENU
    menu[1]--;
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 2) {
    // 1 1 2
    // TIME MENU
    hour -= 1;
    if (hour > 23 || hour < 0) {
      hour = 23;
    }
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 3) {
    // 1 1 3
    // TIME MENU
    minute -= 1;
    if (minute > 59 || minute < 0) {
      minute = 59;
    }
  } else if (menu[0] == 1 && menu[1] == 1 && menu[2] == 4) {
    // 1 1 4
    // TIME MENU
    second -= 1;
    if (second > 59 || second < 0) {
      second = 59;
    }
  } else if (menu[0] == 1 && menu[1] == 2 && menu[2] == 0) {
    // 1 2 0
    // COUNTER LIMIT MENU
    menu[1]--;
  } else if (menu[0] == 1 && menu[1] == 2 && menu[2] == 2) {
    // 1 2 2
    // COUNTER LIMIT MENU
    limit -= 1;
    if (limit > 999999 || limit < 0) {
      limit = 999999;
    }
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 0) {
    // 1 3 0
    // RESET COUNTER MENU
    menu[1]--;
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 1) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    menu[2] = 2;
  } else if (menu[0] == 1 && menu[1] == 3 && menu[2] == 2) {
    // 1 3 2
    // RESET COUNTER MENU
    // No
    menu[2] = 1;
  }
}
void DownReleased() {
  Serial.println("Down Released");
}
void EnterPressed() {

  Serial.println("Enter Pressed");
  if (menu[0] == 0 && menu[1] == 0 && menu[2] == 0) {
    menu[0] = 1;
    menu[1] = 0;
    menu[2] = 0;
  } else {
    if (menu[1] >= 0 && menu[0] == 1) {
      menu[2] += 1;
    } else {
      menu[1] = 0;
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);

  display.println("");
  display.print("DATE ");
  display.print(20);
  display.print(" ");
  display.print(3);
  display.print(" ");
  display.println(2023);
  String _test ="Test";
  display.print(_test);
  display.print("TIME ");
  // display.print(20);
  display.print(12);
  display.print(" ");
  display.print(3);
  display.print(" ");
  display.println(59);
  // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
  display.print("Totol = ");
  display.println(total);
  display.display();
}
void EnterReleased() {
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

//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SH110X_WHITE);
//   display.setCursor(0, 0);

//   display.println("");
//   display.print("DATE ");
//   display.print(20);
//   display.print(" ");
//   display.print(3);
//   display.print(" ");
//   display.println(2023);
//   String _test ="Test";
//   display.print(_test);
//   display.print("TIME ");
//   // display.print(20);
//   display.print(12);
//   display.print(" ");
//   display.print(3);
//   display.print(" ");
//   display.println(59);
//   // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
//   display.print("Totol = ");
//   display.println(total);
//   display.display();

// display.clearDisplay();
// display.setTextSize(1);
// // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
// display.setTextColor(SH110X_WHITE);
// display.setCursor(0, 0);
// display.println("");
// display.setTextColor(SH110X_BLACK, SH110X_WHITE);
// display.println("DATE");
// display.setTextColor(SH110X_WHITE);
// display.println("TIME");
// display.println("COUNTER LIMIT");
// display.println("RESET COUNTER");
// display.println("ALARM");
// display.display();