#include <SPI.h>
#include <SD.h>
#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>
#include <EEPROM.h>
#include "DataTypeProgram.h"

#define SD_CS 10    // | Mega 2560
#define SD_MOSI 11  // | COPI 51
#define SD_MISO 12  // | CIPO 50
#define SD_SCK 13   // |  CSK 52

#define BUTTON_ESC_PIN 4
#define BUTTON_UP_PIN 6
#define BUTTON_DOWN_PIN 7
#define BUTTON_ENTER_PIN 5
#define BUZZER_PIN 9

void ButtonEscPressed(void);
void ButtonEscReleased(void);
TcBUTTON buttonEsc(BUTTON_ESC_PIN, ButtonEscPressed, ButtonEscReleased);

void ButtonEnterPressed(void);
void ButtonEnterReleased(void);
TcBUTTON buttonEnter(BUTTON_ENTER_PIN, ButtonEnterPressed, ButtonEnterReleased);

void ButtonUpPressed(void);
void ButtonUpReleased(void);
TcBUTTON buttonUp(BUTTON_UP_PIN, ButtonUpPressed, ButtonUpReleased);

void ButtonDownPressed(void);
void ButtonDownReleased(void);
TcBUTTON buttonDown(BUTTON_DOWN_PIN, ButtonDownPressed, ButtonDownReleased);

#define LED_MES_PIN A1
TcPINOUT ledMes(LED_MES_PIN);

#define LED_GREEN_PIN A2
TcPINOUT ledGreen(LED_GREEN_PIN);

#define LED_RED_PIN A3
TcPINOUT ledRed(LED_RED_PIN);

DS1302 rtc(2, 3, 8);

#define LCD_ADDRESS 0x27
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

const int Vin = 5;      // voltage at 5V pin of arduino
float Volt = 0;         // voltage at A0 pin of arduino
const float R1 = 3300;  // value of known resistance
float R2 = 0;           // value of unknown resistance
float oldR2 = 0;        // value of unknown resistance
int a2d_data = 0;
float buffer = 0;

enum BUTTON_STATE {
  ESC,
  ENTER,
  UP,
  DOWN,
  NONE
};
BUTTON_STATE currentButtonState = BUTTON_STATE::NONE;
bool IsPress = false;
int countPress = 0;

bool IsChangeMenu = false;

uint8_t day, month, hour, minute, second;
uint16_t year = 2021;
// max min value for time
uint8_t maxHour = 23;
uint8_t minHour = 0;
uint8_t maxMinute = 59;
uint8_t minMinute = 0;
uint8_t maxSecond = 59;
uint8_t minSecond = 0;
// max min value for date
const uint8_t maxDay = 31;
const uint8_t minDay = 1;
const uint8_t maxMonth = 12;
const uint8_t minMonth = 1;
const uint16_t maxYear = 2050;
const uint16_t minYear = 2021;

unsigned long lastTimeDisplay = 0;
unsigned long lastTimeMain = 0;
unsigned long lastTimeBtnPress = 0;
enum Display {
  Home,
  Settings,
};

Display display = Home;
Display oldDisplay = Settings;

enum Setings {
  TIME,
  DATE,
  RANGE,
  BUZZER,
  Lots,
  LotSize,
  MAX,
  MIN,
};

Setings setings = TIME;
Setings oldSetings = TIME;

enum Pages {
  Index,
  Get,
  Show,
  Hour,
  Minute,
  Second,
  Day,
  Month,
  Year,
  Save,
  None,
  Yes,
  No,
  ON,
  OFF,
  Set
};
Pages pages = Hour;
Pages oldPages = Hour;

DataTypeProgram dataProgram;
// HistoryData history;

Time t;
int countUseMenu = 0;

// --------------------- SETUP --------------------- //
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Wire.begin();
  lcd.begin();
  lcd.backlight();

  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card not found");
    lcd.setCursor(0, 0);
    lcd.print("SD Card");
    lcd.setCursor(0, 1);
    lcd.print("Not found!!");
    while (1)
      ;
  }
  // writeLong(0,9000);
  // writeLong(4,10000);
  dataProgram.readFromSD();
  // delay(2000);
  // Example of updating a value
  // dataProgram.updateValue("Lots", "15");
  // dataProgram.MinResistance = readLong(0);
  // dataProgram.MaxResistance = readLong(4);
  // dataProgram.IsBuzzer = readLong(20);
  // Update DATE
  UpdateDateTime();
  // Set DATE TO FILENAME
  dataProgram.FileName = "h" + String(year) + String(month) + String(day);
  Serial.println("done.");
  lcd.clear();
  String strartTXT = "Loading.";
  for(int i =0; i<6;i++){
    lcd.setCursor(0, 0);
    strartTXT += ".";
    lcd.print(strartTXT);
    delay(200);
  }
 lcd.clear();
}

void loop() {
  buttonEsc.update();
  buttonEnter.update();
  buttonUp.update();
  buttonDown.update();
  // Read the current time
  a2d_data = analogRead(A0);  // read the value from the sensor
  buffer = a2d_data * Vin;    // buffer the data for smoothing
  Volt = (buffer) / 1024.0;
  buffer = Volt / (Vin - Volt);
  R2 = R1 * buffer;  // calculate the resistance of the sensor

  if (display != oldDisplay || setings != oldSetings || pages != oldPages || IsChangeMenu) {
    lcd.noBlink();
    // Serial.println("UpdateLCD");
    oldDisplay = display;
    oldSetings = setings;
    oldPages = pages;
    IsChangeMenu = false;
    UpdateLCD();
  }

  if (millis() - lastTimeMain >= 1000) {
    // BTN Sleep
    if (display == Settings) {
      countUseMenu++;
      if (countUseMenu >= 30) {
        lcd.clear();
        display = Home;
        setings = TIME;
        countUseMenu = 0;
      }

      if (IsPress) {
        countPress++;
        // Serial.println("countPress: " + String(countPress));
      } else {
        countPress = 0;
      }
    }
    lastTimeMain = millis();
  }

  if (millis() - lastTimeBtnPress > 50) {
    // Serial.println("R2: " + String(R2));
    // ---------------------------- BUTTON PRESS ---------------------------------- //
    if (IsPress && countPress > 1 && display == Settings && setings != Index) {
      if (currentButtonState == BUTTON_STATE::UP || currentButtonState == BUTTON_STATE::DOWN) {
        Serial.println("Press: " + String(countPress));
        BUTTON_PUSHES(currentButtonState);
      }
    }
    lastTimeBtnPress = millis();
  }
}

void UpdateDateTime() {
  t = rtc.getTime();
  day = t.date;
  month = t.mon;
  year = t.year;
  hour = t.hour;
  minute = t.min;
  second = t.sec;
}
void UpdateLCD() {

  switch (display) {
    case Home:
      DisplayHome();
      break;
    case Settings:
      DisplaySettings();
      break;
    default:
      display = Home;
      break;
  }
}
int digitResistor = 0;
int oldDigitResistor = 0;

int GetDigit(float input) {
  if (input < 10)
    return 1;
  if (input < 100)
    return 2;
  if (input < 1000)
    return 3;
  if (input < 10000)
    return 4;
  if (input < 100000)
    return 5;
  if (input < 1000000)
    return 6;
  return 7;
}

String GetUnit(float input) {
  if (input > 1000 && input < 1000000) {
    return String(input / 1000.0, 2) + "k";
  } else if (input >= 1000000) {
    return String(input / 1000000.0, 2) + "M";
  } else {
    return String(input, 2);
  }
}

String result = "";
String oldResult = "A";
void DisplayHome() {
  if (millis() - lastTimeDisplay > 100) {
    // ---------------------------- Display Home ---------------------------------- //
    String result = CalculateResistor(R2);
    if (digitResistor != oldDigitResistor || result != oldResult) {
      lcd.clear();
      oldDigitResistor = digitResistor;
      oldResult = result;
    }
    digitResistor = GetDigit(R2);

    if (R2 < 500000) {
      lcd.setCursor(0, 0);
      lcd.print("R: ");
      lcd.print(GetUnit(R2));
      lcd.print("Ohm");
      lcd.setCursor(0, 1);
      lcd.print("Res :" + result);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("R: ");
      lcd.print("Overload");
      lcd.print("");
      lcd.setCursor(0, 1);
      lcd.print("Res :" + result);
    }
    // ---------------------------- END Display Home ---------------------------------- //
    lastTimeDisplay = millis();
  } else if (millis() < 100) {
    lastTimeDisplay = millis();
  }
  IsChangeMenu = true;
}

bool IsRangeChange = false;
bool IsRangeReset = false;

String CalculateResistor(float input) {
  if (IsRange(input, dataProgram.MinResistance, dataProgram.MaxResistance)) {
    if (IsRangeReset == true) {
      if (dataProgram.IsBuzzer) {
        //
        UpdateDateTime();
        // Update Lots
        dataProgram.Lots++;
        dataProgram.writeToSD();

        dataProgram.Measurement = String(R2);
        dataProgram.Min = String(dataProgram.MinResistance);
        dataProgram.Max = String(dataProgram.MinResistance);
        dataProgram.Date = String(day) + "/" + String(month) + "/" + String(year);
        dataProgram.Time = String(hour) + "/" + String(minute) + "/" + String(second);
        dataProgram.HLots = String(dataProgram.Lots) + "/" + String(dataProgram.LotSize);
        dataProgram.writeHistoryToSD();
        tone(BUZZER_PIN, 600, 100);
        delay(200);
        tone(BUZZER_PIN, 600, 100);
      }

      IsRangeReset = false;
    }
    ledMes.on();
    ledGreen.on();
    ledRed.off();
    return "OK";
  }
  if (IsRangeReset == false) {

    IsRangeReset = true;
  }

  if (input > 500000) {
    // OFF LED ALL
    ledMes.off();
    ledGreen.off();
    ledRed.off();
    return "OUT";
  }
  ledMes.off();
  ledGreen.off();
  ledRed.on();
  return "NG";
}

bool IsRange(float input, float min, float max) {
  if (input > min && input < max) {
    return true;
  }
  return false;
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

void DisplaySettings() {
  lcd.noCursor();
  lcd.noBlink();
  lcd.clear();
  switch (setings) {
    case TIME:
      DisplayTime();
      break;
    case DATE:
      DisplayDate();
      break;
    case RANGE:
      DisplayRange();
      break;
    case Setings::Lots:
      DisplayLots();
      break;
    case Setings::LotSize:
      DisplayLotSize();
      break;
    case MIN:
      DisplayMin();
      break;
    case MAX:
      DisplayMax();
      break;
    default:
      setings = TIME;
      break;
  }
}
void DisplayTime() {
  switch (pages) {
    case Index:
      lcd.setCursor(0, 0);
      lcd.print(">Time");
      lcd.setCursor(0, 1);
      lcd.print(" Date");
      break;
    case Get:
      t = rtc.getTime();
      day = t.date;
      month = t.mon;
      year = t.year;
      hour = t.hour;
      minute = t.min;
      second = t.sec;
      pages = Show;
      IsChangeMenu = true;
      // goto case Show;
      break;
    case Show:
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      lcd.print(hour);
      lcd.print(":");
      lcd.print(minute);
      lcd.print(":");
      lcd.print(second);
      lcd.setCursor(0, 1);
      lcd.print("Date: ");
      lcd.print(day);
      lcd.print("/");
      lcd.print(month);
      lcd.print("/");
      lcd.print(year);
      break;

    case Hour:
      lcd.setCursor(0, 0);
      lcd.blink();
      lcd.cursor();
      lcd.print("SET Hour: ");
      lcd.print(hour);
      lcd.print(" ");
      break;
    case Minute:
      lcd.setCursor(0, 0);
      lcd.blink();
      lcd.cursor();
      lcd.print("SET Minute: ");
      lcd.print(minute);
      lcd.print(" ");
      break;
    case Second:
      lcd.blink();
      lcd.cursor();
      lcd.setCursor(0, 0);
      lcd.print("SET Second: ");
      lcd.print(second);
      lcd.print(" ");
      break;
    case Save:
      // SET TIME
      rtc.setTime(hour, minute, second);
      lcd.setCursor(0, 0);
      // off brink and cursor
      lcd.noBlink();
      lcd.noCursor();
      lcd.print("Saving...");
      delay(1000);
      pages = Show;
      IsChangeMenu = true;
      break;
  }
}

void DisplayDate() {
  switch (pages) {
    case Index:
      lcd.setCursor(0, 0);
      lcd.print(" Time");
      lcd.setCursor(0, 1);
      lcd.print(">Date");
      break;
    case Get:
      t = rtc.getTime();
      day = t.date;
      month = t.mon;
      year = t.year;
      hour = t.hour;
      minute = t.min;
      second = t.sec;
      pages = Show;
      IsChangeMenu = true;
      // goto case Show;
      break;
    case Show:
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      lcd.print(hour);
      lcd.print(":");
      lcd.print(minute);
      lcd.print(":");
      lcd.print(second);
      lcd.setCursor(0, 1);
      lcd.print("Date: ");
      lcd.print(day);
      lcd.print("/");
      lcd.print(month);
      lcd.print("/");
      lcd.print(year);
      break;
    case Day:
      lcd.setCursor(0, 0);
      lcd.blink();
      lcd.cursor();
      lcd.print("SET Day: ");
      lcd.print(day);
      lcd.print(" ");
      break;
    case Month:
      lcd.setCursor(0, 0);
      lcd.blink();
      lcd.cursor();
      lcd.print("SET Month: ");
      lcd.print(month);
      lcd.print(" ");
      break;
    case Year:
      lcd.setCursor(0, 0);
      lcd.blink();
      lcd.cursor();
      lcd.print("SET Year: ");
      lcd.print(year);
      lcd.print(" ");
      break;
    case Save:
      // SET TIME
      rtc.setDate(day, month, year);
      lcd.setCursor(0, 0);
      // off brink and cursor
      lcd.noBlink();
      lcd.noCursor();
      lcd.print("Saving...");
      delay(1000);
      pages = Show;
      IsChangeMenu = true;
      break;
  }
}
uint8_t IsBuzzerSelect = false;

void DisplayRange() {
  switch (pages) {
    case Index:
      // _min = dataProgram.MinResistance;
      lcd.setCursor(0, 0);
      lcd.print(">BUZZER : " + GetBuzzer());
      lcd.setCursor(0, 1);
      lcd.print(" ACC :" + String(dataProgram.Lots) + "/" + String(dataProgram.LotSize));
      break;
    case ON:
      lcd.setCursor(0, 0);
      lcd.print(">ON");
      lcd.setCursor(0, 1);
      lcd.print(" OFF");
      IsBuzzerSelect = 1;
      break;
    case OFF:
      lcd.setCursor(0, 0);
      lcd.print(" ON");
      lcd.setCursor(0, 1);
      lcd.print(">OFF");
      IsBuzzerSelect = 0;
      break;
    case Save:
      lcd.setCursor(0, 0);
      lcd.print("Saving...");
      pages = Index;
      // IsBuzzer = select;
      dataProgram.IsBuzzer = IsBuzzerSelect;
      dataProgram.writeToSD();
      // writeLong(20, dataProgram.IsBuzzer);
      delay(1000);
      IsChangeMenu = true;
      break;
    default:
      break;
  }
}
bool IsReset = false;
void DisplayLots() {
  switch (pages) {
    case Index:
      lcd.setCursor(0, 0);
      lcd.print(" BUZZER : " + GetBuzzer());
      lcd.setCursor(0, 1);
      lcd.print(">ACC :" + String(dataProgram.Lots) + "/" + String(dataProgram.LotSize));
      break;
    case No:
      lcd.setCursor(0, 0);
      lcd.print("RESET :");
      lcd.setCursor(0, 1);
      lcd.cursor();
      lcd.blink();
      lcd.print(">NO  YES");
      IsReset = false;
      break;
    case Yes:
      lcd.setCursor(0, 0);
      lcd.print("RESET :");
      lcd.setCursor(0, 1);
      lcd.cursor();
      lcd.blink();
      lcd.print(" NO >YES");
      IsReset = true;
      break;
    case Save:
      lcd.setCursor(0, 0);
      lcd.print("Saving...");
      pages = Index;
      // IsBuzzer = select;
      if (IsReset) {
        dataProgram.Lots = 0;
        dataProgram.writeToSD();
      }
      delay(1000);
      IsChangeMenu = true;
      break;
  }
}
int _lotSize = 0;
int _min = 0;
void DisplayLotSize() {
  switch (pages) {
    case Index:
      _min = dataProgram.MinResistance;
      lcd.setCursor(0, 0);
      lcd.print(">Count : " + String(dataProgram.LotSize));
      lcd.setCursor(0, 1);
      _lotSize = dataProgram.LotSize;
      lcd.print(" MIN :" + String(_min) + " Ohm");
      break;
    case Set:
      lcd.setCursor(0, 0);
      lcd.print("SET LOT SIZE :");
      lcd.setCursor(0, 1);
      lcd.cursor();
      lcd.blink();
      lcd.print(_lotSize);
      break;
    case Save:
      lcd.setCursor(0, 0);
      lcd.print("Saving...");
      pages = Index;
      dataProgram.LotSize = _lotSize;
      dataProgram.writeToSD();
      delay(1000);
      IsChangeMenu = true;
      break;
  }
}
void DisplayMin() {
  switch (pages) {
    case Index:
      lcd.setCursor(0, 0);
      lcd.print(" Count : " + String(dataProgram.LotSize));
      lcd.setCursor(0, 1);
      _min = dataProgram.MinResistance;
      lcd.print(">MIN :" + String(_min) + " Ohm");
      break;
    case Set:
      lcd.setCursor(0, 0);
      lcd.print("SET MIN :");
      lcd.setCursor(0, 1);
      lcd.cursor();
      lcd.blink();
      lcd.print(_min);
      break;
    case Save:
      lcd.setCursor(0, 0);
      lcd.print("Saving...");
      pages = Index;
      // IsBuzzer = select;
      dataProgram.MinResistance = _min;
      // writeLong(0, dataProgram.MinResistance);
      dataProgram.writeToSD();
      delay(1000);
      IsChangeMenu = true;
      break;
  }
}

int _max = 0;
void DisplayMax() {
  switch (pages) {
    case Index:
      _max = dataProgram.MaxResistance;
      lcd.setCursor(0, 0);
      lcd.print(">MAX :" + String(_max) + " Ohm");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      break;
    case Set:
      lcd.setCursor(0, 0);
      lcd.print("SET MAX :");
      lcd.setCursor(0, 1);
      lcd.cursor();
      lcd.blink();
      lcd.print(_max);
      break;
    case Save:
      lcd.setCursor(0, 0);
      lcd.print("Saving...");
      pages = Index;
      // IsBuzzer = select;
      dataProgram.MaxResistance = _max;
      // writeLong(4, dataProgram.MaxResistance);
      dataProgram.writeToSD();
      delay(1000);
      IsChangeMenu = true;
      break;
  }
}

String GetBuzzer() {
  if (dataProgram.IsBuzzer == 1) {
    return "ON";
  }
  return "OFF";
}
// -------------------- Button Esc --------------------
void ButtonEscPressed(void) {
  BUTTON_PUSHES(ESC);
  currentButtonState = BUTTON_STATE::ESC;
  IsPress = true;
  countPress = 0;
  // Serial.println("ButtonEscPressed: ");
}

void ButtonEscReleased(void) {
  IsPress = false;
}

// -------------------- Button Enter --------------------
void ButtonEnterPressed(void) {
  BUTTON_PUSHES(ENTER);
  currentButtonState = BUTTON_STATE::ENTER;
  IsPress = true;
  countPress = 0;
}

void ButtonEnterReleased(void) {
  IsPress = false;
}
// -------------------- Button Up --------------------
void ButtonUpPressed(void) {
  BUTTON_PUSHES(UP);
  currentButtonState = BUTTON_STATE::UP;
  IsPress = true;
  countPress = 0;
}

void ButtonUpReleased(void) {
  IsPress = false;
}
// -------------------- Button Down --------------------
void ButtonDownPressed(void) {
  BUTTON_PUSHES(DOWN);
  currentButtonState = BUTTON_STATE::DOWN;
  IsPress = true;
  countPress = 0;
}

void ButtonDownReleased(void) {
  IsPress = false;
}

// -------------------- FUNCTION -------------------- //
void BUTTON_PUSHES(BUTTON_STATE button) {
  countUseMenu = 0;
  switch (button) {
    case ESC:
      Serial.println("ESC");
      if (display == Home) {
        display = Settings;
        pages = Index;
      } else if (display == Settings) {
        if (pages != Index) {
          pages = Index;
        } else {
          display = Home;
        }
      }
      lcd.clear();
      break;
    case ENTER:

      if (display == Settings) {
        switch (setings) {
          case TIME:
            if (pages == Index) {
              Serial.println("Index -> Get");
              pages = Get;
            } else if (pages == Show) {
              Serial.println("Show -> Hour");
              pages = Hour;
            } else if (pages == Hour) {
              Serial.println("Hour -> Minute");
              pages = Minute;
            } else if (pages == Minute) {
              Serial.println("Minute -> Second");
              pages = Second;
            } else if (pages == Second) {
              Serial.println("Second -> Save");
              pages = Save;
            } else if (pages == Save) {
              pages = None;
            }
            break;
          case DATE:
            if (pages == Index) {
              pages = Get;
            } else if (pages == Show) {
              pages = Day;
            } else if (pages == Day) {
              pages = Month;
            } else if (pages == Month) {
              pages = Year;
            } else if (pages == Year) {
              pages = Save;
            } else if (pages == Save) {
              pages = None;
            }
            break;
          case RANGE:
            if (pages == Index) {
              if (dataProgram.IsBuzzer == 1) {
                pages = ON;
              } else {
                pages = OFF;
              }
            } else if (pages == ON || pages == OFF) {
              pages = Save;
            } else if (pages == Save) {
              pages = Index;
            }
            break;
          case Lots:
            if (pages == Index) {
              pages = No;
            } else if (pages == No || pages == Yes) {
              pages = Save;
            } else if (pages == Save) {
              pages = Index;
            }
            break;
          case LotSize:
            if (pages == Index) {
              pages = Set;
            } else if (pages == Set) {
              pages = Save;
            } else if (pages == Save) {
              pages = Index;
            }
            break;
          case MIN:
            if (pages == Index) {
              pages = Set;
            } else if (pages == Set) {
              pages = Save;
            } else if (pages == Save) {
              pages = Index;
            }
            break;
          case MAX:
            if (pages == Index) {
              pages = Set;
            } else if (pages == Set) {
              pages = Save;
            } else if (pages == Save) {
              pages = Index;
            }
            break;
          default:
            break;
        }
      }
      break;
    case UP:
      if (pages == Index) {
        switch (setings) {
          case TIME:
            setings = Setings::MAX;
            break;
          case DATE:
            setings = Setings::TIME;
            break;
          case RANGE:
            setings = Setings::DATE;
            break;
          case Setings::Lots:
            setings = Setings::RANGE;
            Serial.println("Setings::Lots->Setings::RANGE");
            break;
          case Setings::LotSize:
            setings = Setings::Lots;
            break;
          case MIN:
            setings = Setings::LotSize;
            break;
          case MAX:
            setings = Setings::MIN;
            break;
          default:
            break;
        }
      } else {
        switch (setings) {
          case TIME:
            switch (pages) {
              case Hour:
                if (hour < maxHour) {
                  hour++;
                } else {
                  hour = minHour;
                }
                break;
              case Minute:
                if (minute < maxMinute) {
                  minute++;
                } else {
                  minute = minMinute;
                }
                break;
              case Second:
                if (second < maxSecond) {
                  second++;
                } else {
                  second = minSecond;
                }
                break;
              default:
                break;
            }
            break;
          case DATE:
            switch (pages) {
              case Day:
                if (day < maxDay) {
                  day++;
                } else {
                  day = minDay;
                }
                break;
              case Month:
                if (month < maxMonth) {
                  month++;
                } else {
                  month = minMonth;
                }
                break;
              case Year:
                if (year < maxYear) {
                  year++;
                } else {
                  year = minYear;
                }
                break;
              default:
                break;
            }
            break;
          case RANGE:
            switch (pages) {
              case ON:
                pages = OFF;
                break;
              case OFF:
                pages = ON;
                break;
              default:
                break;
            }
            break;
          case Lots:
            switch (pages) {
              case No:
                pages = Yes;
                break;
              case Yes:
                pages = No;
                break;
              default:
                break;
            }
            break;
          case LotSize:
            switch (pages) {
              case Set:
                _lotSize++;
                break;
              default:
                break;
            }
            break;
          case MIN:
            switch (pages) {
              case Set:
                _min++;
                break;
              default:
                break;
            }
            break;

          case MAX:
            switch (pages) {
              case Set:
                _max++;
                break;
              default:
                break;
            }
            break;
          default:
            break;
        }
        IsChangeMenu = true;
      }
      Serial.println("UP");
      break;
    case DOWN:
      if (pages == Index) {
        switch (setings) {
          case TIME:
            setings = DATE;
            break;
          case DATE:
            setings = RANGE;
            break;
          case RANGE:
            setings = Setings::Lots;
            break;
          case Setings::Lots:
            setings = Setings::LotSize;
            break;
          case Setings::LotSize:
            setings = MIN;
            break;
          case MIN:
            setings = MAX;
            break;
          case MAX:
            setings = TIME;
            break;
          default:
            break;
        }
      } else {
        switch (setings) {
          case TIME:
            switch (pages) {
              case Hour:
                if (hour > minHour) {
                  hour--;
                } else {
                  hour = maxHour;
                }
                break;
              case Minute:
                if (minute > minMinute) {
                  minute--;
                } else {
                  minute = maxMinute;
                }
                break;
              case Second:
                if (second > minSecond) {
                  second--;
                } else {
                  second = maxSecond;
                }
                break;
              default:
                break;
            }
            break;
          case DATE:
            switch (pages) {
              case Day:
                if (day > minDay) {
                  day--;
                } else {
                  day = maxDay;
                }
                break;
              case Month:
                if (month > minMonth) {
                  month--;
                } else {
                  month = maxMonth;
                }
                break;
              case Year:
                if (year > minYear) {
                  year--;
                } else {
                  year = maxYear;
                }
                break;
              default:
                break;
            }
            break;
          case RANGE:
            switch (pages) {
              case ON:
                pages = OFF;
                break;
              case OFF:
                pages = ON;
                break;
              default:
                break;
            }
            break;
          case Lots:
            switch (pages) {
              case No:
                pages = Yes;
                break;
              case Yes:
                pages = No;
                break;
              default:
                break;
            }
            break;
          case LotSize:
            switch (pages) {
              case Set:
                _lotSize--;
                if (_lotSize < 0) {
                  _lotSize = 0;
                }
                break;
              default:
                break;
            }
            break;
          case MIN:
            switch (pages) {
              case Set:
                _min--;
                if (_min < 0) {
                  _min = 0;
                }
                break;
              default:
                break;
            }
            break;
          case MAX:
            switch (pages) {
              case Set:
                _max--;
                if (_max < 0) {
                  _max = 0;
                }
                break;
              default:
                break;
            }
            break;

          default:
            break;
        }
        IsChangeMenu = true;
      }
      Serial.println("DOWN");
      break;
    default:
      break;
  }
}
String GetStatus() {
  String status = "";

  switch (display) {
    case Home:
      status = "Home";
      break;
    case Settings:
      status = "Settings";
      break;
    default:
      break;
  }

  switch (setings) {
    case TIME:
      status += "Time";
      break;
    case DATE:
      status += "Date";
      break;
    case RANGE:
      status += "Range";
      break;
    default:
      break;
  }

  switch (pages) {
    case Index:
      status += " Index";
      break;
    case Get:
      status += " Get";
      break;
    case Show:
      status += " Show";
      break;
    case Hour:
      status += " Hour";
      break;
    case Minute:
      status += " Minute";
      break;
    case Second:
      status += " Second";
      break;
    case Day:
      status += " Day";
      break;
    case Month:
      status += " Month";
      break;
    case Year:
      status += " Year";
      break;
    case Save:
      status += " Save";
      break;
    case None:
      status += " None";
      break;
    default:
      break;
  }

  return status;
}