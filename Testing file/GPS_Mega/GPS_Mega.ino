//Connect with pin 14 and 15
#include <TinyGPS.h>
//long   lat,lon; // create variable for latitude and longitude object
float lat, lon;
TinyGPS gps; // create gps object

void setup() {
  Serial.begin(9600); // connect serial
  Serial.println("The GPS Received Signal:");
  Serial3.begin(9600); // connect gps sensor

}

void loop() {
  while (Serial3.available())
  { // check for gps data

    if (gps.encode(Serial3.read())) // encode gps data
    {
      gps.f_get_position(&lat, &lon); // get latitude and longitude

      Serial.print("Position: ");

      //Latitude
      Serial.print("Latitude: ");
      Serial.print(lat, 6);

      Serial.print(",");

      //Longitude
      Serial.print("Longitude: ");
      Serial.println(lon, 6);

    }
  }
}
