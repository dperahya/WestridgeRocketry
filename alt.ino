#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

#define SEALEVELPRESSURE_HPA (1013.5)
Adafruit_BMP3XX bmp;

void altSet() {
  if (!bmp.begin_I2C()) while (1); 
  
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

float alt() {
  if (! bmp.performReading()) {
    return 0.0;
  }
  
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}