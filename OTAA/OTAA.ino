#include <LoRaWan.h>
#include <TinyGPS++.h>

unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];
float lat;
float lng;

//Battery
const int pin_battery_status  = A5;
const int pin_battery_voltage = A4;

// The TinyGPS++ object
TinyGPSPlus gps;

void setup(void)
{
    //SerialUSB.begin(115200);
    //while(!SerialUSB);
    
    //Battery
    pinMode(pin_battery_status, INPUT);

    //GPS
    Serial.begin(9600);
    
    
    lora.init();
    
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    //SerialUSB.print(buffer); 
    
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    //SerialUSB.print(buffer);

    // setKey(char *NwkSKey, char *AppSKey, char *AppKey)
    //lora.setKey("2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C");
    lora.setKey(NULL, NULL, "2B7E151628AED2A6ABF7158809CF4F3C");

    lora.setDeciveMode(LWOTAA);
    lora.setDataRate(DR2, EU868);

    // Spark NZ Pre-AS923 channel-plan config support
    lora.setChannel(0, 923.2);
    lora.setChannel(1, 923.4);
    lora.setChannel(2, 923.4);

    lora.setPort(1);  // Logical LoRWAN fPort for NaaS north-bound API message tagging to AS
    
    lora.setReceiceWindowFirst(0);
    lora.setReceiceWindowSecond(926.4, DR2);
    //lora.setReceiceWindowSecond(926.4, SF12, BW125);
    lora.setReceiceWindowDelay(JOIN_ACCEPT_DELAY2,6000);
    
    lora.setPower(20);
    lora.setAdaptiveDataRate(true);
    
    while(!lora.setOTAAJoin(JOIN));
}

void loop(void)
{   
    while(!lora.setOTAAJoin(JOIN));
    //Battery ->
    
    int a = analogRead(pin_battery_voltage);
    float v = a/1023.0*3.3*11.0;        // there's an 1M and 100k resistor divider
    //SerialUSB.print("BAT:");
    //SerialUSB.println(v, 2);
    
    //Battery <-
    bool result = false;


    //GPS ->
    // This sketch displays information every time a new sentence is correctly encoded.

    /*
    while(Serial.available()) {
      gps.encode(Serial.read());
    }
    */
    
    //displayInfo();

    lat = gps.location.lat();
    lng = gps.location.lng();
    String gpsString ="";  //make sure the string is empty if its not

    gpsString += String(int(lat)) + "." + String(getDecimal(lat)) + ",";
    gpsString += String(int(lng)) + "." + String(getDecimal(lng)) + ",";
    //gpsString += String(int(v)) + "." + String(getDecimal(v));
    gpsString += String(lora.getBatteryVoltage());

    char charVal[gpsString.length()+1];                      //initialise character array to store the values
    gpsString.toCharArray(charVal,gpsString.length()+1);
    
    //GPS <-
    result = lora.transferPacket(charVal, 10);
    //result = lora.transferPacketWithConfirmed("Hello Spark!!", 10);
     // result = lora.transferPacket(data, 10, 10);

    
    /*
    if(result)
    {
        short length;
        short rssi;
        
        memset(buffer, 0, 256);
        length = lora.receivePacket(buffer, 256, &rssi);
        
        if(length)
        {
            SerialUSB.print("Length is: ");
            SerialUSB.println(length);
            SerialUSB.print("RSSI is: ");
            SerialUSB.println(rssi);
            SerialUSB.print("Data is: ");
            for(unsigned char i = 0; i < length; i ++)
            {
                SerialUSB.print("0x");
                SerialUSB.print(buffer[i], HEX);
                SerialUSB.print(" ");
            }
            SerialUSB.println();
        }
    }

    */
    delay(10000);
    smartdelay(10000);
}


static void smartdelay(unsigned long ms) {
    unsigned long start = millis();
    do {
    while (Serial.available())
      gps.encode(Serial.read());
    } while (millis() - start < ms);
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

/*
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

*/

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
