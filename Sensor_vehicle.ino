#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

// The TinyGPS object
TinyGPS gps;

SoftwareSerial Bluetooth(10, 11); //RX || TX
SoftwareSerial gsm(13, 12); //RX || TX

#define m11 31 //M1a
#define m12 33 //M1b
#define m21 35 //M2a
#define m22 37 //M2b

#define buzzer 7 //buzzer
#define MQ2 A0 //Alcohol sensor
#define Sleep_IR A2 //Sleep Sensor

#define front_led 40 //Front LED
#define back_led 42 //Back LED

#define echoPin 24 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 26 //attach pin D3 Arduino to pin Trig of HC-SR04

#define Speed 3 // PWM Speed Control

char bluetooth;
int sensorThres = 400;
int Ready = 0;
int sleep_Ready = 0;
int sleep_sms=1;
int permit = 1;
int sms = 1;
int gps_status = 1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
float lat, lon;

int pwmOutput;

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int max_distance = 120; //Set distance in cm
int max_speed = 150;
int speed_check = 1;


void setup()
{
  Serial.begin(9600);
  Bluetooth.begin(9600);
  gsm.begin(9600);
  Serial3.begin(9600); // connect gps sensor

  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(m21, OUTPUT);
  pinMode(m22, OUTPUT);
  pinMode(A2, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(MQ2, INPUT);
  pinMode(Sleep_IR, INPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  digitalWrite(front_led, HIGH);
  digitalWrite(back_led, HIGH);

  lcd.begin();

  pinMode (front_led, OUTPUT);
  pinMode (back_led, OUTPUT);

  digitalWrite(m11, LOW);
  digitalWrite(m12, LOW);
  digitalWrite(m21, LOW);
  digitalWrite(m22, LOW);
  digitalWrite(buzzer, LOW);

  lcd.backlight();
  lcd.print("Sensor Vehicle");
  Serial.println("Ready to GO!");
}

void loop()
{
  sonar();
  alcohol_check();
  sleep_check();
  bluetooth_rx();
  GPS();
  object_check();
}
void alcohol_check()
{
  int analogSensor = analogRead(MQ2);
  if (analogSensor > sensorThres)
  {
    Stop();
    indicate_led_blink();
    buzzer_on();
    permit = 0;
    Ready = 1;
    Serial.println("Alcohol Detected");

    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Alcohol Detected");
    if (sms == 1)
    {
      gsm.listen();
      Send_alcohol_Message();
      sms = 0;
    }
  }
  else
  {
    buzzer_off();
    indicate_led_on();
    sms = 1;

    if (Ready == 1)
    {
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Ready to GO!");
      Ready = 0;
      permit = 1;
    }

    //To print GPS data on Serial Monitor
    //    Serial.print(lat, 6);
    //    Serial.print(",");
    //    Serial.print(lon, 6);
    //    Serial.println("");

  }
  delay(100);
}
void sleep_check()
{
  int sleepSensor = analogRead(Sleep_IR);
   Serial.println(sleepSensor);
  if (sleepSensor < 500)
  {
    Stop();
    buzzer_on();
    indicate_led_blink();
    permit = 0;
    sleep_Ready = 1;
    if (sleep_sms == 1)
    {
      gsm.listen();
      Send_sleep_Message();
      sleep_sms = 0;
    }
    Serial.println("Sleep Detected");
    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Sleep Detected");
  }
  else
  {
    if (sleep_Ready == 1)
    {
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Ready to GO!!");
      sleep_Ready = 0;
      buzzer_off();
      indicate_led_on();
      permit = 1;
      sleep_sms = 1;
    }
  }
  delay(100);
}
void Send_alcohol_Message()
{
  gsm.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  gsm.println("AT+CMGS=\"+917978480276\"\r");
  delay(1000);
  gsm.println("Drink and Drive Alert!!");// The SMS text you want to send
  delay(100);
  gsm.print("https://www.google.com/maps/place/");
  delay(100);
  gsm.print(lat, 6);
  delay(100);
  gsm.print(",");
  delay(100);
  gsm.print(lon, 6);
  delay(100);
  gsm.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module
  delay(100);
}
void Send_sleep_Message()
{
  gsm.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  gsm.println("AT+CMGS=\"+917978480276\"\r");
  delay(1000);
  gsm.println("Sleep Alert!!");// The SMS text you want to send
  delay(100);
  gsm.print("https://www.google.com/maps/place/");
  delay(100);
  gsm.print(lat, 6);
  delay(100);
  gsm.print(",");
  delay(100);
  gsm.print(lon, 6);
  delay(100);
  gsm.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module
  delay(100);
}
void GPS()
{
  while (Serial3.available())// check for gps data
  {
    if (gps.encode(Serial3.read())) // encode gps data
    {
      gps.f_get_position(&lat, &lon); // get latitude and longitude
    }
  }
}
void bluetooth_rx()
{
  Bluetooth.listen();
  //Serial.println("Waiting for bluetooth data");
  while (Bluetooth.available())
  {
    bluetooth = Bluetooth.read();
    if (((bluetooth == 'f') || (bluetooth == 'F')) && permit == 1) // forward
    {
      Serial.println("Forward");
      speed_check = 1;
      forward();
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Forward");

    }
    else if (((bluetooth == 'l') || (bluetooth == 'L')) && permit == 1) //left
    {
      Serial.println("Left");
      speed_check = 0;
      left();
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Left");

    }
    else if (((bluetooth == 'r') || (bluetooth == 'R')) && permit == 1) //right
    {
      Serial.println("Right");
      speed_check = 0;
      right();
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Right");

    }

    else if (((bluetooth == 'b') || (bluetooth == 'B')) && permit == 1) //backward
    {
      Serial.println("Backward");
      speed_check = 0;
      backward();
      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Backward");

    }
    else if ((bluetooth == 's') || (bluetooth == 'S')) //stop
    {
      Serial.println("Stop");
      Stop();

      delay(100);
      // Clear the screen
      lcd.clear();
      lcd.print("Stop");

    }
  }
  delay(100);
}
void forward()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12, LOW);
  digitalWrite(m21, HIGH);
  digitalWrite(m22, LOW);
}
void right()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12, HIGH);
  digitalWrite(m21, HIGH);
  digitalWrite(m22, LOW);
}
void backward()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12, HIGH);
  digitalWrite(m21, LOW);
  digitalWrite(m22, HIGH);
}
void left()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12, LOW);
  digitalWrite(m21, LOW);
  digitalWrite(m22, HIGH);
}
void Stop()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12, LOW);
  digitalWrite(m21, LOW);
  digitalWrite(m22, LOW);
}
void buzzer_on()
{
  digitalWrite(buzzer, HIGH);
}
void buzzer_off()
{
  digitalWrite(buzzer, LOW);
}
void indicate_led_on()
{
  digitalWrite(front_led, HIGH);
  digitalWrite(back_led, HIGH);
}
void indicate_led_blink()
{
  digitalWrite(front_led, HIGH);
  digitalWrite(back_led, HIGH);
  delay(30);
  digitalWrite(front_led, LOW);
  digitalWrite(back_led, LOW);
}
void sonar()
{
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  //Serial.println(distance);
}
void speed_control()
{
  pwmOutput = map(distance, 5, max_distance, 0, max_speed);
  analogWrite(Speed, pwmOutput);
}
void object_check()
{
  if ((distance > 5) && (distance < max_distance) && (speed_check == 1))
  {
    
    speed_control();
    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Brake Activated");
  }
  else if ((distance > max_distance)|| (speed_check == 0))
  {
    analogWrite(Speed, max_speed);
    delay(100);
    // Clear the screen
    lcd.clear();
    lcd.print("Ready to GO!");
  }
}
