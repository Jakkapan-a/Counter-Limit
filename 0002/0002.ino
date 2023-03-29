#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// #include <DS3231.h>
#include <EEPROM.h>

// DS3231 myRTC;
bool century = false;
bool h12Flag;
bool pmFlag;

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
// #define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
#define i2c_Address 0x3c
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Sensor Pin D6
#define SensorPin 6
void SensorPressed(void);
TcBUTTON Sensor(SensorPin, SensorPressed, NULL);

// Button esc, up, down, enter = A0 A1 A2 A3
#define EscPin A0
#define UpPin A1
#define DownPin A2
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


int myMenu[3] = {0, 0, 0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting");
  Wire.begin();
  delay(250); // wait for the OLED to power up
  display.begin(0x3C,true);  // Address 0x3C default
  delay(100);
  display.clearDisplay();
  display.display();
  Serial.println("Started");

}

void loop() {
  Sensor.update();
  Esc.update();
  Up.update();
  Down.update();
  Enter.update();
}

void SensorPressed() {
  total++;
}

void AlarmPIsOn(bool state) {
  Serial.print("Alarm is ");
  Serial.println(state ? "On" : "Off");
}

void EscPressed() {

  _mainMenuFunc();
}
void EscReleased() {
  // Serial.println("Esc Released");
}
void UpPressed() {
if (myMenu[0] == 1 && myMenu[1] != -1 && myMenu[2] == 0) {
    myMenu[1]++;
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
    myMenu[1]++;
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
    myMenu[1]++;
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
    myMenu[1]++;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 1) {
    // 1 3 1
    // RESET COUNTER MENU
    // Yes
    myMenu[2] = 2;
  } else if (myMenu[0] == 1 && myMenu[1] == 3 && myMenu[2] == 2) {
    // 1 3 2
    myMenu[2] = 1;
  }
  
  if(myMenu[1] < 0){
    myMenu[1] = 9;
  }else if(myMenu[1] > 9){
    myMenu[1] = 0;
  }
  _mainMenuFunc();
}
void UpReleased() {
}
void DownPressed() {
  if (myMenu[0] == 1 && myMenu[1] != -1 && myMenu[2] == 0) {
      myMenu[1]--;
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
      myMenu[1]--;
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
      myMenu[1]--;
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
      myMenu[1]--;
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

    if(myMenu[1] < 0){
      myMenu[1] = 9;
    }else if(myMenu[1] > 9){
      myMenu[1] = 0;
    }
    
  _mainMenuFunc();
}
void DownReleased() {
  Serial.println("Down Released");
}
void EnterPressed() {
  Serial.println("Enter Pressed");
  if(myMenu[0] == 0 && myMenu[1] == 0 && myMenu[2] == 0){
    myMenu[0] = 1;
    myMenu[1] = 0;
    myMenu[2] = 0;
  }else{
    if(myMenu[1] >= 0 && myMenu[0] == 1){
      myMenu[2] += 1;
      if(myMenu[2] >9){
        myMenu[2] = 0;
      }
    }else{
      myMenu[1] = 0;
      }
  }

}
void EnterReleased() {
   _mainMenuFunc();
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

void _mainMenuFunc() {
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  // // // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
  display.setTextColor(SH110X_WHITE);

  display.println(" ");
  display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  display.println(" DATE ");
  display.setTextColor(SH110X_WHITE);
  display.println(" TIME ");
  // display.println(" COUNTER LIMIT ");
  // display.println("RESET COUNTER");
  // display.println("ALARMS");
  // display.println(date);
  Serial.print("Menu Code = ");
  Serial.println(getMenuCode());
  display.display();
  // delay(100);

}

int getMenuCode(){
  char temp[10];
  char _menuCode[10] = "";
  for (int i = 0; i < 3; i++) {
    itoa(myMenu[i], temp, 10); // แปลงค่า int เป็น char array
    strcat(_menuCode, temp); // ต่อ char array
  }
  return atoi(_menuCode);
}

void _timer() {
  if (millis() - lastTime > 1000) {
    lastTime = millis();
  } else if (millis() < 1000) {
    lastTime = millis();
  }
}
