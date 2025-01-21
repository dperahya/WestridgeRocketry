#include <Adafruit_LSM6DSO32.h>


Adafruit_LSM6DSO32 dso32;

// struct AccelDataStructure {
//   float x;
//   float y;
//   float z;
// };


void accelSetup() {
  
  if (!dso32.begin_I2C()) {
    Serial.println("Failed to initialized LSM chip.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("LSM chip initialized.");

  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
  dso32.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);  
  

}

AccelDataStructure accel() {
  sensors_event_t accel, gyro, temp;
  dso32.getEvent(&accel, &gyro, &temp);

  AccelDataStructure accelData;
  accelData.x = accel.acceleration.x;
  accelData.y = accel.acceleration.y;
  accelData.z = accel.acceleration.z;

  return accelData;
}
