#include <LSM6DSOX.h>
#include <Adafruit_LSM6DSO32.h>

Adafruit_LSM6DSO32 dso32;
void IMUsetup() {
  // if (!IMU.begin()) {
  //   Serial.println("Failed to initialize IMU!");

  //   while (1);
  // }

  // Serial.print("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Serial.println();
  // Serial.print("Gyroscope sample rate = ");
  // Serial.print(IMU.gyroscopeSampleRate());
  // Serial.println(" Hz");
  // Serial.println();

  if (!dso32.begin_I2C()) {
    // if (!dso32.begin_SPI(LSM_CS)) {
    // if (!dso32.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    // Serial.println("Failed to find LSM6DSO32 chip");
    while (1) {
      delay(10);
    }
  }

}

void IMUreadAccel(IMU_data_container& accel_data) {
  float x, y, z;

  if (dso32.accelerationAvailable()) {
    dso32.readAcceleration(x, y, z);
  }
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.println();

  accel_data.x = x; accel_data.y = y; accel_data.z = z;
}

void IMUreadGyro(IMU_data_container& gyro_data) {
  float x, y, z;

  if (dso32.gyroscopeAvailable()) {
    dso32.readGyroscope(x, y, z);
  }
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.println();

  gyro_data.x = x; gyro_data.y = y; gyro_data.z = z;
}

void IMUreadTemp(float& temp) {
  float temp_float;
  if (IMU.temperatureAvailable()) {
    IMU.readTemperatureFloat(temp_float);
  }
  
  temp = temp_float;
}

// The three functions above WORK but they produce numbers that don't make sense (i.e. z-acceleration not being 1g)

void IMUread(IMU_data_container& accel_data, IMU_data_container& gyro_data, float& temp_data) {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  dso32.getEvent(&accel, &gyro, &temp);
  // Serial.print(accel.acceleration.x);
  // Serial.print("\t");
  // Serial.print(accel.acceleration.y);
  // Serial.print("\t");
  // Serial.print(accel.acceleration.z);
  // Serial.println();

  // Serial.print(gyro.gyro.x);
  // Serial.print("\t");
  // Serial.print(gyro.gyro.y);
  // Serial.print("\t");
  // Serial.print(gyro.gyro.z);
  // Serial.println();

  // Serial.println(dso32.temperature);

  accel_data.x = accel.acceleration.x; accel_data.y = accel.acceleration.y; accel_data.z = accel.acceleration.z;
  gyro_data.x = gyro.gyro.x; gyro_data.y = gyro.gyro.y; gyro_data.z = gyro.gyro.z;
  temp_data = temp.temperature;
}