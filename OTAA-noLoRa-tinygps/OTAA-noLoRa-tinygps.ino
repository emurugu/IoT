#include <TinyGPS.h>

float lat;
float lng;

//Battery
const int pin_battery_status  = A5;
const int pin_battery_voltage = A4;

// The TinyGPS++ object
TinyGPS gps;

void setup(void)
{
    SerialUSB.begin(115200);
    while(!SerialUSB);
    
    //Battery
    pinMode(pin_battery_status, INPUT);

    //GPS
    Serial.begin(9600);

}

void loop(void)
{   
    //GPS ->
    // This sketch displays information every time a new sentence is correctly encoded.
    while(Serial.available() > 0) {
      gps.encode(Serial.read());
    }
    displayInfo();

    lat = gps.location.lat();
    lng = gps.location.lng();
    String gpsString ="";  //make sure the string is empty if its not

    gpsString += String(int(lat)) + "." + String(getDecimal(lat)) + ",";
    gpsString += String(int(lng)) + "." + String(getDecimal(lng)) + ",";
    //gpsString += String(int(v)) + "." + String(getDecimal(v));
    //gpsString += String(lora.getBatteryVoltage());

    char charVal[gpsString.length()+1];                      //initialise character array to store the values
    gpsString.toCharArray(charVal,gpsString.length()+1);

    SerialUSB.println(charVal);
    /*
    while(Serial.available())
    {
        SerialUSB.write(Serial.read());
    }
    */
    //GPS <-
    
    //delay(10000);
}



//function to extract decimal part of float
long getDecimal(float val)
{
  int intPart = int(val);
  long decPart = 1000*(val-intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places. 
                                    //Change to match the number of decimal places you need
  if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
  else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
  else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
}


//GPS Specific
void displayInfo()
{
  SerialUSB.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    SerialUSB.print(gps.location.lat(), 6);
    SerialUSB.print(F(","));
    SerialUSB.print(gps.location.lng(), 6);
  }
  else
  {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    SerialUSB.print(gps.date.month());
    SerialUSB.print(F("/"));
    SerialUSB.print(gps.date.day());
    SerialUSB.print(F("/"));
    SerialUSB.print(gps.date.year());
  }
  else
  {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.hour());
    SerialUSB.print(F(":"));
    if (gps.time.minute() < 10) SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.minute());
    SerialUSB.print(F(":"));
    if (gps.time.second() < 10) SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.second());
    SerialUSB.print(F("."));
    if (gps.time.centisecond() < 10) SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.centisecond());
  }
  else
  {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.println();
}



/*
    while (Serial.available() > 0) {
      if (gps.encode(Serial.read())) {
        if (gps.location.isValid()) {
          SerialUSB.print(gps.location.lat(), 6);
          SerialUSB.print(F(","));
          SerialUSB.print(gps.location.lng(), 6);
          SerialUSB.println();
        } else {
          SerialUSB.println(F("INVALID LOCATION"));
        }
      }
    }
    */
    /*
    while (Serial.available() > 0) {
      if (gps.encode(Serial.read())) {
        if (gps.location.isValid()) {
          displayInfo();
        }
      }
    }
*/
