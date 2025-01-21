#include <SD.h> //include SD card library
#include <Adafruit_BMP3XX.h> //include BMP (altimeter) library
#include <bmp3.h> //include BMP library
#include <bmp3_defs.h> //include BMP library


File sdFile; //define type File as "sdFile"

//define data structure to hold X, Y, Z components of accelerometer readings
struct AccelDataStructure {
  float x;
  float y;
  float z;
};

//define AccelDataStructure as being the format of the function call accel()
AccelDataStructure accel();

//initialize altitude variable
float currAlt = 0.00;
float prevAlt = 0.00;
float initAlt = 0.00; //initial altitude

//explosiveCharge is the PIN that the explosiveCharge is wired to
int explosiveCharge = 8;

//initialize variables for recovery (ejection)
bool checkFalling = false;
float apogee = 0.00; //highest altitude; so far, 0m
float heightFallen = 0.00;
bool haveDeployed = false;

//define properties of the altitude-recording arrays
const int max_size = 2; //the master array has a MAXIMUM of two sub-arrays
const int max_size_sub = 20; //the two sub-arrays can have a MAXIMUM of 20 entries
float altArrays[max_size][max_size_sub] = {0};

int currSize = 0;

int currSizeSub1 = 0;
int currSizeSub2 = 0;

float sub1_avg = 0;
float sub2_avg = 0;

void setup() {
  delay(10);
  Wire.begin();
  Serial.begin(115200);
  while(!Serial){
    delay(10);
  }
  Serial.println("Serial begin");
  // if (!SD.begin(7)) {
    Serial.println("SD not working");
  }

  sdFile = SD.open("data.txt", FILE_WRITE);
  sdFile.println(F("CODE LAST UPDATED 11/18/24"));
  sdFile.println();
  sdFile.close();

  accelSetup();
  altSetup();
  alt();
  accel();

  initAlt = alt();
 
  pinMode(explosiveCharge, OUTPUT); 
  digitalWrite(explosiveCharge, LOW);

}

void loop() {
  
  sdFile = SD.open("data.txt", FILE_WRITE);

  prevAlt = currAlt;
  currAlt = alt();
  sdFile.print("Current Altitude (currAlt): ");
  sdFile.println(currAlt);
  Serial.print("Current Altitude (currAlt): ");
  Serial.println(currAlt);

  AccelDataStructure accelData = accel();
  sdFile.print("Accel X: ");
  sdFile.print(accelData.x);
  sdFile.print(" Y: ");
  sdFile.print(accelData.y);
  sdFile.print(" Z: ");
  sdFile.println(accelData.z);

  Serial.print("Accel X: ");
  Serial.print(accelData.x);
  Serial.print(" Y: ");
  Serial.print(accelData.y);
  Serial.print(" Z: ");
  Serial.println(accelData.z);
  

  // Serial.println(currSize);

  if (currSize < max_size) {
    // Serial.println("Inside array loop");
    if (currSizeSub1 < max_size_sub) {
      altArrays[0][currSizeSub1] = currAlt;
      currSizeSub1++;
      if (currSizeSub1 == 20) {
        currSize = 1;
      }
    } else if (currSizeSub2 < max_size_sub) {
      altArrays[1][currSizeSub2] = currAlt;
      currSizeSub2++;
      if (currSizeSub2 == 20) {
        currSize = 2;
      }
    } 
  } else {
    

    if (!haveDeployed) {
      for (int i = 0; i < max_size; i++) {
        for (int x = 0; x < max_size_sub; x++) {
          if (i == 0) {
            sub1_avg += altArrays[i][x];
          } else if (i == 1) {
            sub2_avg += altArrays[i][x];
          }
        }
      }
      sub1_avg /= 20;
      sub2_avg /= 20;
      checkFalling = checkDescendingOrder();
      if (checkFalling && sub2_avg < sub1_avg) {
        haveDeployed = true;
        Serial.println("DEPLOY");
      }
    }

    for (int i = 0; i < max_size_sub; i++) {
      altArrays[0][i] = altArrays[1][i];  // Clear altArrays[0]
      altArrays[1][i] = 0;
      
    }
    currSizeSub2 = 0;
    currSize = 1;


  }


  // if array not full, open array[0]
  // push data
  // for loop to fill 20 slots
  // --second var for subarray size?

  //float heightFallen = apogee - currAlt;

  checkFalling = prevAlt > currAlt;




  if (checkFalling && !haveDeployed) {
    sdFile.print("heightFallen: ");
    sdFile.println(heightFallen);
    sdFile.print("apogee: ");
    sdFile.println(apogee);
    if (heightFallen >= 1) {
      haveDeployed = true;
      sdFile.println("---------------------");
      sdFile.println("   DEPLOYING!!!!!");
      sdFile.println("---------------------");
    }
  } else if (!haveDeployed && apogee <= currAlt) {
    apogee = currAlt;
  }

  double msElasped = millis();
  String minString = String(msElasped / 60000);
  if (minString[minString.length() - 2] == '0' && minString[minString.length() - 1] == '0') {
    sdFile.print(F("mins elasped: "));
    sdFile.println(msElasped / 60000);
  }


  sdFile.println();
  sdFile.close();

  delay(200); // 1/100 second

}

bool checkDescendingOrder() {
  if (currSize < max_size) return false; // Don't check if array isn't full

  for (int d = 0; d < max_size; d++) {
    for (int i = 0; i < max_size - 1; i++) {
      if (altArrays[d][i] <= altArrays[d][i + 1]) {
        return false; // If any value is not strictly greater than the next, return false
      }
    }
  }
  
  return true; // All values are in descending order
}

