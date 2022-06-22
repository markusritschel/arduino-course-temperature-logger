/***********************************************************
 * Read a DS18B20 temperature sensor
 * Author: Markus Ritschel
 * E-Mail: git@markusritschel.de
 * ************************************************************/
// Load required libraries
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>


// Data wire is plugged into DIGITAL PIN on the Arduino
#define ONE_WIRE_BUS 2
const String logfile = "temperature.log";
float tempC;
const int group_id = 0;


// setup instances
RTC_DS1307 rtc;                  // Setup RTC instance and make available as `rtc`
OneWire oneWire(ONE_WIRE_BUS);        // Setup a oneWire instance to communicate with any OneWire device
DallasTemperature sensors(&oneWire);  // Pass oneWire reference to DallasTemperature library


void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);   // Start Serial communication

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  // automatically set the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Initialize the SD card module (connects internally to pin 10)
  if (!SD.begin(10)) {
    Serial.println("# SD card failed, or not present");
    return;     // don't do anything more:
  } else {
    Serial.println("# SD card initialized");
  }

  // Locate sensors on the bus
  static int deviceCount;
  Serial.print("# Locating devices on pin ");
  Serial.print(ONE_WIRE_BUS);
  Serial.println("");
}

void loop(void)
{ 
  digitalWrite(LED_BUILTIN, HIGH);

  // Send command to all the sensors for temperature conversion
  sensors.requestTemperatures(); 

  // Read temperature data from sensor 
  tempC = sensors.getTempCByIndex(0);

  Serial.print(getISOtime());
  Serial.print(", ");
  Serial.print(twodigits(group_id));
  Serial.print(", ");
  Serial.print(tempC);
  Serial.println("");

  File myFile = SD.open(logfile, FILE_WRITE);
  if (myFile) {
    myFile.print(getISOtime());
    myFile.print(", ");
    myFile.print(twodigits(group_id));
    myFile.print(", ");
    myFile.print(tempC);
    myFile.println("");
    myFile.close();
  }

  digitalWrite(LED_BUILTIN, LOW); 

  delay(1000);
}


/******************************************
Helper function to format datetime elements 
as two digit (leading zeros) string

Parameters: Integer value
Returns: String repr. of two digit number
******************************************/
String twodigits(int x) {
    String output = "";
    if (x < 10) {
        output += "0";
        output += x;
    } else {
        output = x;
    }
    return output;
}
/*****************************************/


/******************************************
Helper function to generate timestamp in
ISO8601 format.
******************************************/
String getISOtime() {
  DateTime now = rtc.now();
  String ISO8601_time = "";

  ISO8601_time +=  now.year();
  ISO8601_time +=  "-";
  ISO8601_time +=  twodigits(now.month());
  ISO8601_time +=  "-";
  ISO8601_time +=  twodigits(now.day());
  ISO8601_time +=  "T";
  ISO8601_time +=  twodigits(now.hour());
  ISO8601_time +=  ":";
  ISO8601_time +=  twodigits(now.minute());
  ISO8601_time +=  ":";
  ISO8601_time +=  twodigits(now.second());
  ISO8601_time +=  "Z";

  return ISO8601_time;
}
/*****************************************/