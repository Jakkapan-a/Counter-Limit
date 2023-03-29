#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup()
{
	// initialize the LCD
	lcd.begin();
}

void loop()
{
	bool blinking = true;
	lcd.cursor_off();

	while (1) {
		if (blinking) {
			lcd.clear();
			lcd.print("No cursor blink");
			lcd.noBlink();
			blinking = false;
		} else {
			// Cursor blink All text

			lcd.clear();
      lcd.blink();
			lcd.print("Cursor blink");
      lcd.print("A");
			blinking = true;
		}
		delay(2000);
	}
}
