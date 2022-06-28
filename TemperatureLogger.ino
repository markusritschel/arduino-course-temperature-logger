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


// Define variables and constants
#define ONE_WIRE_BUS 2                    // Data wire is plugged into DIGITAL PIN on the Arduino
const String logfile = "temperature.log"; // Name of the log file on the SD card
const int group_id = 1;                   // Your group ID to identify your logger
unsigned long currentMillis;
unsigned long lastMillis;
const unsigned long measurementInterval = 20*1000; // Measurement interval in milliseconds


// setup instances
RTC_DS1307 rtc;                       // Setup RTC instance and make available as `rtc`
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
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) - TimeSpan(0,1,59,50));   // adjust clock via local time to UTC
  Serial.println(F("RTC adjusted to UTC time!"));

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

  // NOTE: When writing to SD, comment out non-data strings with `#`
  write2SD("# Sampling rate: ");
  write2SD(String(measurementInterval/1000));
  writeln2SD(" seconds");
  writeln2SD("# timestamp, group_id, temperature (spot), temperature (average)");

  lastMillis = millis();
}

void loop(void)
{ 
  static unsigned long deltaMillis;
  static int sampleIndex = 0;
  static float tempC;
  static float tempAvg = 0;
  static float tempSpot = 0;

  // Send command to all the sensors for temperature conversion
  sensors.requestTemperatures(); 
  // Read temperature data from sensor 
  tempC = sensors.getTempCByIndex(0);
  tempAvg += tempC;
  sampleIndex += 1;

  currentMillis = millis();
  deltaMillis = currentMillis-lastMillis;

  // If interval has passed, write output
  if (deltaMillis >= (measurementInterval-200))
  {
    // Measurement done
    digitalWrite(LED_BUILTIN, HIGH);
    // Divide by number of samples to get average value
    tempAvg /= sampleIndex;
    Serial.print("# [DEBUG] Average measurement over ");
    Serial.print(sampleIndex);
    Serial.println(" samples");

    // # timestamp, sensor_id (2 digits), T in °C (2 digits; spot), T in °C (2 digits; avg)
    write2SD(getISOtime());
    write2SD(", ");
    write2SD(twodigits(group_id));
    write2SD(", ");
    write2SD(String(tempSpot));
    write2SD(", ");
    write2SD(String(tempAvg));
    writeln2SD("");

    // Reset variables
    tempAvg = 0;
    tempSpot = 0;
    sampleIndex = 0;
    lastMillis = currentMillis;

    digitalWrite(LED_BUILTIN, LOW);
  } else
  // Set spot measurement if about half the time has elapsed (i.e. by around 750ms)
  if ((deltaMillis > measurementInterval/3) and (deltaMillis%(measurementInterval/2) <= 750))
  {
    tempSpot = tempC;
    Serial.print("# [DEBUG] Spot measurement @ ");
    Serial.println(getISOtime());
  }

  delay(200);
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


/******************************************
Helper functions to write to SD card
******************************************/
void write2SD(String dataString) {
  File myFile = SD.open(logfile, FILE_WRITE);
  if (myFile) {
    myFile.print(dataString);
    myFile.close();
  };
  Serial.print(dataString);
}

void writeln2SD(String dataString) {
  File myFile = SD.open(logfile, FILE_WRITE);
  if (myFile) {
    myFile.println(dataString);
    myFile.close();
  };
  Serial.println(dataString);
}
/*****************************************/
