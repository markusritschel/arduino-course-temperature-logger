/***********************************************************
 * Read a DS18B20 temperature sensor
 * Author: Markus Ritschel
 * E-Mail: git@markusritschel.de
 * ************************************************************/
// Load required libraries
#include <RTClib.h>


// setup instances
RTC_DS1307 rtc;                  // Setup RTC instance and make available as `rtc`


void setup(void)
{
  Serial.begin(9600);   // Start Serial communication

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  // automatically set the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop(void)
{ 
  Serial.println(getISOtime());
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