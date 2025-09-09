#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_SensorLab.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;


void altSetup() {
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

}

float alt() {
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return -1.0;
  }

  return(bmp.readAltitude(SEALEVELPRESSURE_HPA));

}
