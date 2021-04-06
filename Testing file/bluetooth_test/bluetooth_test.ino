#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char myChar ; 

void setup() {
  Serial.begin(9600);   
  Serial.println("Goodnight moon!");

  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop(){
  while(mySerial.available()){
    myChar = mySerial.read();
    Serial.println(myChar);
  }
  Serial.println("Out side bluetooth while");

  while(Serial.available()){
   myChar = Serial.read();
   mySerial.print(myChar);
  }
}
