//sketch created by Akshay Joseph
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{

	lcd.begin();


	lcd.backlight();
	lcd.clear();
	lcd.setCursor(4,0);
	lcd.print("Hackster");
  delay(2000);

  Serial.begin(9600);
  Serial.println("ready");
}

void loop()
{
}
