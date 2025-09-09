#include <Adafruit_LSM6DSO32.h>
#include <Adafruit_SensorLab.h>


Adafruit_LSM6DSO32 dso32;

// float accelOffsetX = 0;
// float accelOffsetY = 0;
// float accelOffsetZ = 0;

// struct AccelDataStructure {
//   float x;
//   float y;
//   float z;
// };


void accelSetup() {
  // Serial.begin(115200);
  
  
  if (!dso32.begin_I2C()) {
    Serial.println("Failed to initialized LSM chip.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("LSM chip initialized.");

  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
  dso32.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  dso32.setAccelDataRate(LSM6DS_RATE_416_HZ);
  dso32.setGyroDataRate(LSM6DS_RATE_416_HZ);  

  // measureOffsets();
  

}

// void measureOffsets() {
//   sensors_event_t accel, gyro, temp;

//   // Average multiple readings to get a reliable offset
//   float sumX = 0, sumY = 0, sumZ = 0;
//   int numReadings = 100;

//   for (int i = 0; i < numReadings; i++) {
//     dso32.getEvent(&accel, &gyro, &temp);
//     sumX += accel.acceleration.x;
//     sumY += accel.acceleration.y;
//     sumZ += accel.acceleration.z;
//     delay(10);
//   }

//   // Calculate the average offsets
//   accelOffsetX = sumX / numReadings;
//   accelOffsetY = sumY / numReadings;
//   accelOffsetZ = (sumZ / numReadings) - 9.8; // Remove gravity effect
// }

AccelDataStructure accel() {
  sensors_event_t accel, gyro, temp;
  dso32.getEvent(&accel, &gyro, &temp);

  AccelDataStructure accelData;
  accelData.x = accel.acceleration.x;
  accelData.y = accel.acceleration.y;
  accelData.z = accel.acceleration.z;
  // accelData.x = accel.acceleration.x - accelOffsetX;
  // accelData.y = accel.acceleration.y - accelOffsetY;
  // accelData.z = accel.acceleration.z - accelOffsetZ;

  return accelData;
}
