
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GPS.h>

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false

uint32_t timer = millis();
// Set your local UTC offset here in hours (e.g., -7 for PDT)
const int8_t utcOffset = -7;

void setup() {
  Serial.begin(9600);
  LoRa.begin(915E6);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() {
  char c = GPS.read();
  if (GPSECHO && c) Serial.print(c);

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) return;
  }

  if (millis() - timer > 2000) {
    timer = millis();

    if (GPS.fix) {
      int localHour = GPS.hour + utcOffset;
      if (localHour < 0) localHour += 24;
      else if (localHour > 23) localHour -= 24;

      // Construct the time string like the reference code
      String output = "\nTime: ";
      if (localHour < 10) output += '0';
      output += String(localHour) + ":";
      if (GPS.minute < 10) output += '0';
      output += String(GPS.minute) + ":";
      if (GPS.seconds < 10) output += '0';
      output += String(GPS.seconds) + ".";
      // Milliseconds zero padding
      if (GPS.milliseconds < 10) output += "00";
      else if (GPS.milliseconds < 100) output += "0";
      output += String(GPS.milliseconds);

      output += "\nDate: ";
      if (GPS.day < 10) output += '0';
      output += String(GPS.day) + "/";
      if (GPS.month < 10) output += '0';
      output += String(GPS.month) + "/20";
      output += String(GPS.year);

      output += "\nFix: " + String((int)GPS.fix);
      output += " quality: " + String((int)GPS.fixquality);

      output += "\nLocation: ";
      output += String(GPS.latitude, 4) + GPS.lat + ", ";
      output += String(GPS.longitude, 4) + GPS.lon;

      output += "\nSpeed (knots): " + String(GPS.speed);
      output += "\nAngle: " + String(GPS.angle);
      output += "\nAltitude: " + String(GPS.altitude);
      output += "\nSatellites: " + String((int)GPS.satellites);
      output += "\nAntenna status: " + String((int)GPS.antenna);

      // Send via LoRa
      LoRa.beginPacket();
      LoRa.print(output);
      LoRa.endPacket();

      // Also print to Serial Monitor
      Serial.println(output);
    } else {
      String nofix = "No GPS fix";
      LoRa.beginPacket();
      LoRa.print(nofix);
      LoRa.endPacket();
      Serial.println(nofix);
    }
  }
}
