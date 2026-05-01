#include <Adafruit_LSM6DSO32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Arduino_MKRGPS.h>
#include <SPI.h>
#include <LoRa.h>
#include <SD.h>

#define SEALEVELPRESSURE_HPA (1013.25)
const int chipSelect = 7;

Adafruit_LSM6DSO32 dso32;
Adafruit_BMP3XX bmp;

File dataFile;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=== Sensor + GPS + LoRa + SD ===");

  // --- SD CARD ---
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD failed!");
    while (1);
  }
  Serial.println("SD OK");

  //setup text file
  //change to dif file for actual day
  dataFile = SD.open("test2.txt", FILE_WRITE);
  if (dataFile) {
      dataFile.println("time,lat,lon,alt_gps,speed,sat,ax,ay,az,pressure,alt_baro");
      dataFile.close();
      Serial.println("Header Saved to SD!");
  } else {
      Serial.println("SD header write failed!");
  }

  // --- LSM6DSO32 ---
  if (!dso32.begin_I2C()) {
    Serial.println("LSM6DSO32 not found!");
    while (1);
  }
  Serial.println("LSM6DSO32 OK");
  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);

  // --- BMP390 ---
  if (!bmp.begin_I2C()) {
    Serial.println("BMP390 not found!");
    while (1);
  }
  Serial.println("BMP390 OK");

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // --- GPS ---
  if (!GPS.begin()) {
    Serial.println("GPS failed!");
    while (1);
  }
  Serial.println("GPS OK");

  // --- LoRa ---
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed!");
    while (1);
  }
  Serial.println("LoRa OK");
}

void loop() {
  if (GPS.available()) {

    // --- IMU ---
    sensors_event_t accel, gyro, temp;
    dso32.getEvent(&accel, &gyro, &temp);

    // --- BMP ---
    float pressure = 0;
    float bmpAltitude = 0;

    if (bmp.performReading()) {
      pressure = bmp.pressure / 100.0;
      bmpAltitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    }

    // --- GPS ---
    float latitude   = GPS.latitude();
    float longitude  = GPS.longitude();
    float gpsAltitude = GPS.altitude();
    float speed      = GPS.speed();
    int satellites   = GPS.satellites();

    // time stamp
    float time = millis();

    // lora data for sending to lora
    String loraData = "";
    loraData += "Time:" + String(time) + "\n";

    loraData += "LAT:" + String(latitude, 6) + "\n";
    loraData += "LON:" + String(longitude, 6) + "\n";
    loraData += "ALTG:" + String(gpsAltitude) + "\n";
    loraData += "SPD:" + String(speed) + "\n";
    loraData += "SAT:" + String(satellites) + "\n";

    loraData += "AX:" + String(accel.acceleration.x) + "\n";
    loraData += "AY:" + String(accel.acceleration.y) + "\n";
    loraData += "AZ:" + String(accel.acceleration.z) + "\n";

    loraData += "PRES:" + String(pressure) + "\n";
    loraData += "ALTB:" + String(bmpAltitude) + "\n";

    loraData += "---------------------------";

    Serial.println(loraData);

    //SD Logging
    String sdData = "";

    sdData += String(time) + ",";
    sdData += String(latitude, 6) + ",";
    sdData += String(longitude, 6) + ",";
    sdData += String(gpsAltitude) + ",";
    sdData += String(speed) + ",";
    sdData += String(satellites) + ",";
    sdData += String(accel.acceleration.x) + ",";
    sdData += String(accel.acceleration.y) + ",";
    sdData += String(accel.acceleration.z) + ",";
    sdData += String(pressure) + ",";
    sdData += String(bmpAltitude);

    //Log to sd
    Serial.println("Sending + Logging:");
    Serial.println(sdData);

    // --- LoRa ---
    LoRa.beginPacket();
    LoRa.print(loraData);
    LoRa.endPacket();

    // --- SD LOGGING ---
    //change to dif file for actual day
    dataFile = SD.open("test2.txt", FILE_WRITE);

    if (dataFile) {
      dataFile.println(sdData);
      dataFile.close();
      Serial.println("Saved to SD");
    } else {
      Serial.println("SD write failed!");
    }

    Serial.println("-------------------\n");

    delay(2000);
  }
}