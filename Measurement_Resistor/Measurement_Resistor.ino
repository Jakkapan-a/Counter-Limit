#include <TcBUTTON.h>
#include <TcPINOUT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// #include <DS3231.h>
#include <DS1302.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include <SPI.h>
#include <SD.h>

#define SD_CS 10
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK 13

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

DS1302 rtc(2, 3, 8);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int Vin = 5;     // voltage at 5V pin of arduino
float Volt = 0;        // voltage at A0 pin of arduino
const float R1 = 3300; // value of known resistance
float R2 = 0;          // value of unknown resistance
float oldR2 = 0;       // value of unknown resistance
int a2d_data = 0;
float buffer = 0;

// Min and max variables for the resistance range(min 9k max 10k)
float minResistance = 9000;  // 9k
float maxResistance = 10000; // 10k

unsigned long lastTimeDisplay = 0;
unsigned long lastTimeMain = 0;
enum Display
{
    Home,
    Settings,
};

Display display = Home;
Display oldDisplay = Settings;

enum Setings
{
    TIME,
    DATE,
    RANGE,
    BUZZER
};

Setings setings = TIME;
Setings oldSetings = TIME;

enum Pages
{
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
    OFF
};

Pages pages = Hour;
Pages oldPages = Hour;

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

int32_t IsBuzzer = false;
enum BUTTON_STATE
{
    ESC,
    ENTER,
    UP,
    DOWN
};

int countMenu = 0;
int countSubMenu = 0;
// --------------------- Class --------------------- //
class DataTypeProgram
{
public:
    int Lots;
    int LotSize;
    bool IsBuzzer;
    float MinResistance;
    float MaxResistance;
};
// -------------------- Variable -------------------- //
DataTypeProgram dataProgram;
// -------------------- Function -------------------- //
void setup()
{
    Serial.begin(115200);
    Wire.begin();
    lcd.begin();
    lcd.backlight();

    if (!SD.begin(SD_CS))
    {
        Serial.println("SD card not found");
        lcd.setCursor(0, 0);
        lcd.print("SD Card");
        lcd.setCursor(0, 1);
        lcd.print("Not found!!");

        while (1)
            ;
    }
    DisplayHome();
    if (!SD.exists("data.json"))
    {
        Serial.println("data.json does not exist. Creating...");
        writeJsonToSDCreate();
    }
    else
    {
        Serial.println("data.json exists.");
    }
    readJsonFromSD();
    IsBuzzer = readLong(20);
}
Time t;
void loop()
{
    buttonEsc.update();
    buttonEnter.update();
    buttonUp.update();
    buttonDown.update();

    a2d_data = analogRead(A0); // read the value from the sensor
    buffer = a2d_data * Vin;   // buffer the data for smoothing
    Volt = (buffer) / 1024.0;
    buffer = Volt / (Vin - Volt);
    R2 = R1 * buffer; // calculate the resistance of the sensor

    if (display != oldDisplay || setings != oldSetings || pages != oldPages || IsChangeMenu)
    {
        lcd.noBlink();
        // Serial.println("UpdateLCD");
        oldDisplay = display;
        oldSetings = setings;
        oldPages = pages;
        IsChangeMenu = false;
        UpdateLCD();
    }
}
void writeJsonToSDCreate()
{
    File dataFile = SD.open("data.json", FILE_WRITE);

    DynamicJsonDocument doc(1024);
    doc["Lots"] = 0;
    doc["LotSize"] = 0;
    doc["IsBuzzer"] = false;
    doc["MinResistance"] = 9000;
    doc["MaxResistance"] = 10000;

    if (dataFile)
    {
        serializeJson(doc, dataFile);
        dataFile.close();
        Serial.println("JSON data written to SD card.");
    }
    else
    {
        Serial.println("Error opening data.json for writing.");
    }
}

void readJsonFromSD()
{
    File dataFile = SD.open("data.json", FILE_READ);
    if (dataFile)
    {
        DynamicJsonDocument doc(1024);
         DeserializationError error = deserializeJson(doc, dataFile);
        if (error) {
            Serial.println("Failed to read file, using default configuration");
        }

        dataProgram.Lots = doc["Lots"];
        dataProgram.LotSize = doc["LotSize"];
        dataProgram.IsBuzzer = doc["IsBuzzer"];
        dataProgram.MinResistance = doc["MinResistance"];
        dataProgram.MaxResistance = doc["MaxResistance"];

        Serial.println("JSON data read from SD card.");
        Serial.print("Lots: ");
        Serial.println(dataProgram.Lots);
        Serial.print("LotSize: ");
        Serial.println(dataProgram.LotSize);
        Serial.print("IsBuzzer: ");
        Serial.println(dataProgram.IsBuzzer);
        Serial.print("MinResistance: ");
        Serial.println(dataProgram.MinResistance);
        Serial.print("MaxResistance: ");
        Serial.println(dataProgram.MaxResistance);
        dataFile.close();
    }else{
        Serial.println("Error opening data.json for reading.");
    }
}
void updateJsonData()
{
    File dataFile = SD.open("data.json", FILE_READ);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, dataFile);
    dataFile.close();
    if (error)
    {
        Serial.println("Failed to read file");
        return;
    }
    // Modify the contents in memory
    doc["Lots"] = dataProgram.Lots;
    doc["LotSize"] = dataProgram.LotSize;
    doc["IsBuzzer"] = dataProgram.IsBuzzer;
    doc["MinResistance"] = dataProgram.MinResistance;
    doc["MaxResistance"] = dataProgram.MaxResistance;

    // Reopen the file for writing
    dataFile = SD.open("data.json", FILE_WRITE);
    if (!dataFile)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    // Serialize JSON to file
    serializeJson(doc, dataFile);
    dataFile.close();
    Serial.println("JSON data written to SD card.");

}
void UpdateLCD()
{

    switch (display)
    {
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

int GetDigit(float input)
{
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

String GetUnit(float input)
{
    if (input > 1000 && input < 1000000)
    {
        return String(input / 1000.0, 2) + "k";
    }
    else if (input >= 1000000)
    {
        return String(input / 1000000.0, 2) + "M";
    }
    else
    {
        return String(input, 2);
    }
}
String result = "";
String oldResult = "A";
void DisplayHome()
{
    if (millis() - lastTimeDisplay > 100)
    {
        String result = CalculateResistor(R2);
        if (digitResistor != oldDigitResistor || result != oldResult)
        {
            lcd.clear();
            oldDigitResistor = digitResistor;
            oldResult = result;
        }
        digitResistor = GetDigit(R2);

        if (R2 < 600000)
        {
            lcd.setCursor(0, 0);
            lcd.print("R: ");
            lcd.print(GetUnit(R2));
            lcd.print("Ohm");
            lcd.setCursor(0, 1);
            lcd.print("Res :" + result);
        }
        else
        {
            lcd.setCursor(0, 0);
            lcd.print("R: ");
            lcd.print("Overload");
            lcd.print("");
            lcd.setCursor(0, 1);
            lcd.print("Res :" + result);
        }
        lastTimeDisplay = millis();
    }
    else if (millis() < 100)
    {
        lastTimeDisplay = millis();
    }
    IsChangeMenu = true;
}
bool IsRangeChange = false;
bool IsRangeReset = false;

String CalculateResistor(float input)
{

    if (IsRange(input, minResistance, maxResistance))
    {

        if (IsRangeReset == true)
        {
            if(dataProgram.IsBuzzer){
                tone(BUZZER_PIN, 600, 100);
                delay(200);
                tone(BUZZER_PIN, 600, 100);
            }
            // Save data to SD Card

            IsRangeReset = false;
        }
        return "OK";
    }
    if (IsRangeReset == false)
    {
        IsRangeReset = true;
    }
    return "Out";
}

bool IsRange(float input, float min, float max)
{
    if (input > min && input < max)
    {
        return true;
    }
    return false;
}

void writeLong(uint32_t address, uint32_t value)
{
    byte byte1 = (value & 0xFF);
    byte byte2 = ((value >> 8) & 0xFF);
    byte byte3 = ((value >> 16) & 0xFF);
    byte byte4 = ((value >> 24) & 0xFF);

    EEPROM.update(address, byte1);
    EEPROM.update(address + 1, byte2);
    EEPROM.update(address + 2, byte3);
    EEPROM.update(address + 3, byte4);
}

uint32_t readLong(uint32_t address)
{
    uint32_t value = 0;
    value |= EEPROM.read(address);
    value |= ((uint32_t)EEPROM.read(address + 1) << 8);
    value |= ((uint32_t)EEPROM.read(address + 2) << 16);
    value |= ((uint32_t)EEPROM.read(address + 3) << 24);
    return value;
}

void DisplaySettings()
{
    lcd.noCursor();
    lcd.clear();
    switch (setings)
    {
    case TIME:
        DisplayTime();
        break;
    case DATE:
        DisplayDate();
        break;
    case RANGE:
        DisplayRange();
        break;
    default:
        setings = TIME;
        break;
    }
}
void DisplayTime()
{
    switch (pages)
    {
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

void DisplayDate()
{
    switch (pages)
    {
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
bool IsBuzzerSelect = false;
void DisplayRange()
{
    switch (pages)
    {
    case Index:
        lcd.setCursor(0, 0);
        lcd.print(">BUZZER : " + GetBuzzer());
        lcd.setCursor(0, 1);
        lcd.print(" ");
        break;
    case ON:
        lcd.setCursor(0, 0);
        lcd.print(">ON");
        lcd.setCursor(0, 1);
        lcd.print(" OFF");
        IsBuzzerSelect = false;
        break;
    case OFF:
        lcd.setCursor(0, 0);
        lcd.print(" ON");
        lcd.setCursor(0, 1);
        lcd.print(">OFF");
        IsBuzzerSelect = true;
        break;
    case Save:
        lcd.setCursor(0, 0);
        lcd.print("Saving...");
        pages = Index;
        // IsBuzzer = select;
        dataProgram.IsBuzzer = IsBuzzerSelect;
        writeLong(20, IsBuzzer);
        delay(1000);
        IsChangeMenu = true;
        break;
    default:
        break;
    }
}

String GetBuzzer()
{
    if (IsBuzzer)
    {
        return "ON";
    }
    return "OFF";
}

// -------------------- Button Esc --------------------
void ButtonEscPressed(void)
{
    BUTTON_PUSHES(ESC);
}

void ButtonEscReleased(void)
{
}

// -------------------- Button Enter --------------------
void ButtonEnterPressed(void)
{
    BUTTON_PUSHES(ENTER);
}

void ButtonEnterReleased(void)
{
}
// -------------------- Button Up --------------------
void ButtonUpPressed(void)
{
    BUTTON_PUSHES(UP);
}

void ButtonUpReleased(void)
{
}
// -------------------- Button Down --------------------
void ButtonDownPressed(void)
{
    BUTTON_PUSHES(DOWN);
}

void ButtonDownReleased(void)
{
}

// -------------------- FUNCTION -------------------- //
void BUTTON_PUSHES(BUTTON_STATE button)
{
    switch (button)
    {
    case ESC:
        Serial.println("ESC");
        if (display == Home)
        {
            display = Settings;
            pages = Index;
        }
        else if (display == Settings)
        {
            if (pages != Index)
            {
                pages = Index;
            }
            else
            {
                display = Home;
            }
        }
        break;
    case ENTER:

        if (display == Settings)
        {

            switch (setings)
            {
            case TIME:
                if (pages == Index)
                {
                    Serial.println("Index -> Get");
                    pages = Get;
                }
                else if (pages == Show)
                {
                    Serial.println("Show -> Hour");
                    pages = Hour;
                }
                else if (pages == Hour)
                {
                    Serial.println("Hour -> Minute");
                    pages = Minute;
                }
                else if (pages == Minute)
                {
                    Serial.println("Minute -> Second");
                    pages = Second;
                }
                else if (pages == Second)
                {
                    Serial.println("Second -> Save");
                    pages = Save;
                }
                else if (pages == Save)
                {
                    pages = None;
                }
                break;
            case DATE:
                if (pages == Index)
                {
                    pages = Get;
                }
                else if (pages == Show)
                {
                    pages = Day;
                }
                else if (pages == Day)
                {
                    pages = Month;
                }
                else if (pages == Month)
                {
                    pages = Year;
                }
                else if (pages == Year)
                {
                    pages = Save;
                }
                else if (pages == Save)
                {
                    pages = None;
                }
                break;
            case RANGE:
                if (pages == Index)
                {
                    if (dataProgram.IsBuzzer)
                    {
                        pages = ON;
                    }
                    else
                    {
                        pages = OFF;
                    }
                }
                else if (pages == ON || pages == OFF)
                {
                    pages = Save;
                }
                else if (pages == Save)
                {
                    pages = Index;
                }
                break;
            default:
                break;
            }
        }
        break;
    case UP:
        if (pages == Index)
        {
            switch (setings)
            {
            case TIME:
                setings = RANGE;
                break;
            case DATE:
                setings = TIME;
                break;
            case RANGE:
                setings = DATE;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (setings)
            {
            case TIME:
                switch (pages)
                {
                case Hour:
                    if (hour < maxHour)
                    {
                        hour++;
                    }
                    else
                    {
                        hour = minHour;
                    }
                    break;
                case Minute:
                    if (minute < maxMinute)
                    {
                        minute++;
                    }
                    else
                    {
                        minute = minMinute;
                    }
                    break;
                case Second:
                    if (second < maxSecond)
                    {
                        second++;
                    }
                    else
                    {
                        second = minSecond;
                    }
                    break;
                default:
                    break;
                }
                break;
            case DATE:
                switch (pages)
                {
                case Day:
                    if (day < maxDay)
                    {
                        day++;
                    }
                    else
                    {
                        day = minDay;
                    }
                    break;
                case Month:
                    if (month < maxMonth)
                    {
                        month++;
                    }
                    else
                    {
                        month = minMonth;
                    }
                    break;
                case Year:
                    if (year < maxYear)
                    {
                        year++;
                    }
                    else
                    {
                        year = minYear;
                    }
                    break;
                default:
                    break;
                }
                break;
            case RANGE:
                switch (pages)
                {
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
            default:
                break;
            }
            IsChangeMenu = true;
        }
        Serial.println("UP");
        break;
    case DOWN:
        if (pages == Index)
        {
            switch (setings)
            {
            case TIME:
                setings = DATE;
                break;
            case DATE:
                setings = RANGE;
                break;
            case RANGE:
                setings = TIME;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (setings)
            {
            case TIME:
                switch (pages)
                {
                case Hour:
                    if (hour > minHour)
                    {
                        hour--;
                    }
                    else
                    {
                        hour = maxHour;
                    }
                    break;
                case Minute:
                    if (minute > minMinute)
                    {
                        minute--;
                    }
                    else
                    {
                        minute = maxMinute;
                    }
                    break;
                case Second:
                    if (second > minSecond)
                    {
                        second--;
                    }
                    else
                    {
                        second = maxSecond;
                    }
                    break;
                default:
                    break;
                }
                break;
            case DATE:
                switch (pages)
                {
                case Day:
                    if (day > minDay)
                    {
                        day--;
                    }
                    else
                    {
                        day = maxDay;
                    }
                    break;
                case Month:
                    if (month > minMonth)
                    {
                        month--;
                    }
                    else
                    {
                        month = maxMonth;
                    }
                    break;
                case Year:
                    if (year > minYear)
                    {
                        year--;
                    }
                    else
                    {
                        year = maxYear;
                    }
                    break;
                default:
                    break;
                }
                break;
            case RANGE:
                switch (pages)
                {
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
String GetStatus()
{
    String status = "";

    switch (display)
    {
    case Home:
        status = "Home";
        break;
    case Settings:
        status = "Settings";
        break;
    default:
        break;
    }

    switch (setings)
    {
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

    switch (pages)
    {
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