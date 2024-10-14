#include <SD.h>
#include <Wire.h>
#include <bmp3.h>
#include <bmp3_defs.h>
#include <cppQueue.h>
#include <LoRa.h>
#include <MKRWAN_v2.h>

#include "LEDControl.h" 
File sdFile;

float curAlt = 0.00;
float prevAlt = 0.00;
float initAlt = 0.00;

bool checkFalling = false;
float apogee = 0.00;
float heightFallen = 0.00;
bool haveDeployed = false;

const int NLORA_SS = 28;
const int NLORA_RESET = 6;
const int NLORA_DIO0 = 31;

enum state {
  IDLE,
  FLIGHT,
  LANDED,
  TEST
};
state curState;

enum errorCode {
  ERROR_NONE,
  ERROR_ACCEL,
  ERROR_ALT,
  ERROR_SD
};
errorCode curError;

// float accelData[3];

float timeAtFlight;

cppQueue q(sizeof(float), 20, FIFO, true);

struct IMU_data_container {
  float x;
  float y;
  float z;
};

struct data_container{
  unsigned long timestamp;
  float alt;
  IMU_data_container accel_data;
  IMU_data_container gyro_data;
  float temp;
};




const int spiFlashCS = 7;

void setup() {
  // pinMode(NLORA_RESET, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Setup");
  curState = FLIGHT;
  curError = ERROR_NONE;
  Wire.begin();
  
  if (!SD.begin(spiFlashCS)) {
    Serial.println("Something went wrong with SD reader");
      // setLEDMode(1, PULSE, 3);
  };
  if(SD.exists("data.txt")){
    if(SD.remove("data.txt") == true){
      Serial.println("removed data");
    }
  }

  sdFile = SD.open("data.txt", FILE_WRITE); //file name can't be too long
  // sdFile.println(F("CODE LAST UPDATED 5/21/24"));
  // sdFile.println();
  // sdFile.close();

  Serial.println("Taking initial readings");
  IMUsetup();
  // accelSet();
  // altSet();
  // alt(); //first reading always a little ridiculous, don't know why
  // initAlt = alt();//second reading always back to normal
  LoRa.setPins(NLORA_SS, NLORA_RESET, NLORA_DIO0);
  // LoRa.enableCrc();
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    Serial.println("LoRa started");
    // LoRa.enableCrc();
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(500E3);
    LoRa.setCodingRate4(5);
  }

  // pinMode(explosiveCharge, OUTPUT); //tels pin with igniter to be ready to output voltage
  // digitalWrite(explosiveCharge, LOW);  //turns off the voltage from that pin
  Serial.println("Setup complete");
}

void loop() {
  // controlLEDs();
  if (curError != ERROR_NONE) {
    displayError(curError);
  }
  if (curState == IDLE) {
    sdFile = SD.open("data.txt", FILE_WRITE);
    if (!sdFile) {
      Serial.println("Could not open sdFile, trying again in 5 seconds");
      delay(5000);
    } else {
      Serial.println("sdFile succesfully opened, entering flight mode");
      curState = FLIGHT;
      timeAtFlight = millis();
    }
  }
  if (curState == FLIGHT) {
    // accelTake(accelData);
    // for(int i=0; i<3; i++) {
    //   Serial.print(accelData[i]);
    // }
    // Serial.println();

    // prevAlt = curAlt;
    // curAlt = alt();

    IMU_data_container accel_data;
    IMU_data_container gyro_data;
    float temp;

    // IMUreadAccel(accel_data);
    // IMUreadGyro(gyro_data);
    // IMUreadTemp(temp);
    
    IMUread(accel_data, gyro_data, temp);

    Serial.print(accel_data.x);
    Serial.print("\t");
    Serial.print(accel_data.y);
    Serial.print("\t");
    Serial.print(accel_data.z);
    Serial.println();

    Serial.print(gyro_data.x);
    Serial.print("\t");
    Serial.print(gyro_data.y);
    Serial.print("\t");
    Serial.print(gyro_data.z);
    Serial.println();

    Serial.println(temp);
    

    unsigned long timestamp = millis();

    data_container data = {timestamp, curAlt, accel_data, gyro_data, temp};// this is O K

    Serial.println("Sending data");
    Serial.println(timestamp);
    if (LoRa.beginPacket()) {
      // Serial.println("Lora success");
      // LoRa.write((uint8_t*)&curAlt, sizeof(curAlt));
      LoRa.write((uint8_t*)&data, sizeof(data_container));
      if (!LoRa.endPacket()) {
        Serial.println("endPacket failed");
      } else {
        Serial.println("Packet sent");
      }
    } else {
      Serial.println("Something went wrong");
    }
    // double msElapsed = millis() - timeAtFlight;
    // for(int i=0; i<3; i++) {
    //   sdFile.print(accelData[i]);
    //   sdFile.print(",");
    // }
    // sdFile.print(curAlt);
    // sdFile.println();
    // sdFile.print("ms elasped: ");
    // sdFile.println(msElapsed);
    // sdFile.println();

    // Serial.println(curAlt);
    // q.push(&currAlt);
    // if (q.isFull()) {
    //   for (int i = 0; i < 20; i++)
    //       { 
    //         float test;
    //         if (q.peekIdx(&test, i)) {
    //           Serial.println(test);
    //         }
    //       }
    // }
    
    // if (msElapsed > 5000) {
    //   curState = LANDED;
    // }
  }
  if (curState == LANDED) {
    sdFile.close();
    Serial.println("Landed state");
    delay(5000);
  }
  
  // delay(10); // 1/100 second
  Serial.println();
}

void displayError(errorCode error) {
  switch (error) {
        case ERROR_ACCEL:
            Serial.println("Error: Accelerometer failure.");
            // Add display logic here
            break;
        case ERROR_ALT:
            Serial.println("Error: Altimeter failure.");
            break;
        case ERROR_SD:
            Serial.println("Error: SD failure.");
            break;
        default:
            Serial.println("Unknown error.");
    }
    // Reset the error after handling it
    // currentError = ERROR_NONE;
}

void setDebugLED(int led, int state) {
  return;
}
