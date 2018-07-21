

/******************************************************************************
  MinimalistExample.ino

  Marshall Taylor @ SparkFun Electronics
  May 20, 2015
  https://github.com/sparkfun/LSM6DS3_Breakout
  https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library

  Description:
  Most basic example of use.

  Example using the LSM6DS3 with basic settings.  This sketch collects Gyro and
  Accelerometer data every second, then presents it on the serial monitor.

  Resources:
  Uses Wire.h for i2c operation
  Uses SPI.h for SPI operation
  Either can be omitted if not used

  Development environment specifics:
  Arduino IDE 1.6.4
  Teensy loader 1.23

  Hardware connections:
  Connect I2C SDA line to A4
  Connect I2C SCL line to A5
  Connect GND and 3.3v power to the IMU

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).

  Please review the LICENSE.md file included with this example. If you have any questions
  or concerns with licensing, please contact techsupport@sparkfun.com.

  Distributed as-is; no warranty is given.
******************************************************************************/
#include "SparkFunMAX31855k.h"
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"
#include "Adafruit_GPS.h"

// what's the name of the hardware serial port?
#define GPSSerial Serial1
#define GPSECHO  true

struct loggin_pos {
  uint32_t id;
  uint8_t t_day;    //day from GPS
  uint8_t t_month;  //month from GPS
  uint8_t t_year;   //year from GPS
  uint8_t t_h;      //hour
  uint8_t t_min;    //minutes
  uint8_t t_sec;    //seconds
  uint32_t latMSB;  //latitude most significant bytes
  uint32_t latLSB;  //latitude less significant bytes
  uint32_t lonMSB;  //latitude most significant bytes
  uint32_t lonLSB;  //longitude less significant bytes
  uint32_t refsts;  //this is a reference to the loggin_sts
};
struct loggin_sts {
  uint32_t id;      //id of the current logged data
  float Ax;         //acceleration x axis
  float Ay;         //acceleration y axis
  float Az;         //acceleration z axis
  float Gx;         //gyroscope omega*t x axis
  float Gy;         //gyroscope omega*t y axis
  float Gz;         //gyroscope omega*t z axis
  float tboard;     //temperature of the logged board
  float tcouple;    //temperature termocauple
  uint8_t dig1;     //digital input 1
  uint8_t dig2;     //digital input 2
  uint8_t dig3;     //digital input 3
  float an1;         //analog input 1
  float an2;         //analog input 2
  float an3;         //analog input 3
};


// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
uint32_t timer = millis();

// Define SPI Arduino pin numbers (Arduino Pro Mini)
const uint8_t Thermocouple_cs = 9; // Using standard CS line (SS)
// SCK & MISO are defined by Arduiino
const uint8_t VCC = 14; // Powering board straight from Arduino Pro Mini
const uint8_t GND = 15;
uint32_t updateTime = 1000;

LSM6DS3 myIMU; //Default constructor is I2C, addr 0x6B
// Instantiate an instance of the SparkFunMAX31855k class
SparkFunMAX31855k probe(Thermocouple_cs, VCC, GND);

void setup() {
  while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); //relax...
  Serial.println("Processor came out of reset.\n");

  //Call .begin() to configure the IMU
  myIMU.begin();

  // 9600 NMEA is the default baud rate for MTK - some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(100);
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  delay(100);

  Serial.print("\nSTARTING LOGGING....");
  if (GPS.LOCUS_StartLogger())
    Serial.println(" STARTED!");
  else {
    Serial.println(" no response :(, retry the startLogging");
    if (GPS.LOCUS_StartLogger())
      Serial.println(" STARTED!");
    else {
      Serial.println(" no response :(");
    }
  }

}



loggin_pos posLog_step;
loggin_sts statusLog_step;

void loop()
{

  char c = GPS.read();
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();

  // approximately every 0.2 seconds or so, print out the current stats
  if (millis() - timer > 200) {
    if (GPS.fix) {
      timer = millis(); // reset the timer

      posLog_step.id = posLog_step.id + 1;
      posLog_step.t_day = GPS.day;
      posLog_step.t_month = GPS.month;
      posLog_step.t_year = GPS.year;
      posLog_step.t_h = GPS.hour;
      posLog_step.t_min = GPS.minute;
      posLog_step.t_sec = GPS.seconds;
      posLog_step.latMSB = GPS.latitudeDegrees;
      posLog_step.latLSB = GPS.lat;
      posLog_step.lonMSB = GPS.longitudeDegrees;
      posLog_step.lonLSB = GPS.lon;
      posLog_step.refsts = statusLog_step.id;
      //-----------------------
      //=> write into the flash memory
      //-----------------------


      //    Serial.print("\nTime: ");
      //    Serial.print(GPS.hour, DEC); Serial.print(':');
      //    Serial.print(GPS.minute, DEC); Serial.print(':');
      //    Serial.println(GPS.seconds, DEC);
      //    //Serial.println(GPS.milliseconds);
      //    Serial.print("Date: ");
      //    Serial.print(GPS.day, DEC); Serial.print('/');
      //    Serial.print(GPS.month, DEC); Serial.print("/");
      //    Serial.println(GPS.year, DEC);
      //    Serial.print("Fix: "); Serial.print((int)GPS.fix);
      //    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
      //
      //
      //      Serial.print("Location: ");
      //      Serial.print(GPS.latitudeDegrees, 4); Serial.print(GPS.lat);
      //      Serial.print(", ");
      //      Serial.print(GPS.longitudeDegrees, 4); Serial.println(GPS.lon);
      //      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      //      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      //      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
    //    else{
    //       Serial.print("unFIX position");
    //    }
  }


  statusLog_step.id = statusLog_step.id + 1;
  statusLog_step.Ax = myIMU.readFloatAccelX();
  statusLog_step.Ay = myIMU.readFloatAccelY();
  statusLog_step.Az = myIMU.readFloatAccelZ();
  statusLog_step.Gx = myIMU.readFloatGyroX();
  statusLog_step.Gy = myIMU.readFloatGyroY();
  statusLog_step.Gz = myIMU.readFloatGyroZ();
  statusLog_step.tboard = myIMU.readTempC();
  statusLog_step.tcouple = probe.readTempC();
  statusLog_step.dig1 = 0;
  statusLog_step.dig2 = 0;
  statusLog_step.dig3 = 0;
  statusLog_step.an1 = 0;
  statusLog_step.an2 = 0;
  statusLog_step.an3 = 0;
  //-----------------------
  //=> write into the flash memory
  //-----------------------

  //
  ////------------Accelerometer------------//
  ////  Accelerometer X
  //  Serial.print(myIMU.readFloatAccelX(), 4);
  //  Serial.print(" ");
  ////  Accelerometer Y
  //  Serial.print(myIMU.readFloatAccelY(), 4);
  //  Serial.print(" ");
  ////  Accelerometer Z
  //  Serial.print(myIMU.readFloatAccelZ(), 4);
  //  Serial.print(" ");
  ////------------Gyroscope---------------//
  ////  Gyroscope X
  //  Serial.print(myIMU.readFloatGyroX(), 4);
  //  Serial.print(" ");
  ////  Gyroscope Y
  //  Serial.print(myIMU.readFloatGyroY(), 4);
  //  Serial.print(" ");
  ////  Gyroscope Z
  //  Serial.print(myIMU.readFloatGyroZ(), 4);
  //  Serial.print(" ");
  ////--------Circuit Temperature-------//
  //  Serial.print(myIMU.readTempC(), 4);
  //  Serial.print(" ");
  ////-------------Termocouple----------------//
  ////Termocouple Celcius
  //  float temperature = probe.readTempC();
  //  Serial.print(temperature);
  //  Serial.print(" ");

  //END

  delay(50);

}

