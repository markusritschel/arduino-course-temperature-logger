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
  DateTime now = rtc.now();
  Serial.println(now.second());
  delay(1000);
}
