#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(10,11);
void setup(){
 Serial.begin(9600);
 ss.begin(GPSBaud);
}
void loop(){
  //Serial.println("Searching"); 
 // This sketch displays information every time a new sentence is correctly encoded.
 while (ss.available() > 0){
   gps.encode(ss.read());
   if (gps.location.isUpdated()){
     Serial.print("Latitude= "); 
     Serial.print(gps.location.lat(), 6);
     Serial.print(" Longitude= "); 
     Serial.println(gps.location.lng(), 6);
   }
 }
}
