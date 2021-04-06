#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int smokeA0 = A0;
int LED = 7;

int sensorThres = 400;

void setup() {
  pinMode(smokeA0, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
}

void loop() {
  int analogSensor = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Alcohol Detected");
  }
  else
  {
    digitalWrite(LED, LOW);
    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Alcohol not Detected");
  }
  delay(100);
}
