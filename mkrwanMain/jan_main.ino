#include <SD.h> //include SD card library
#include <Adafruit_BMP3XX.h> //include BMP (altimeter) library
#include <bmp3.h> //include BMP library
#include <bmp3_defs.h> //include BMP library
#include <Adafruit_SensorLab.h>
// #include <LoRa.h>


struct AccelDataStructure {
  float x;
  float y;
  float z;
};

// AccelDataStructure accel();

// const int explosiveChargePin = 6;
const int smtCS = 7;

float currAlt = 0.00;
float prevAlt = 0.00;
float initAlt = 0.00;
float apogee = 0.00; //highest altitude; so far, 0m


bool prelimFallCheck = false; //only checks whether currAlt > apogee
bool falling = false;
bool doubleCheckFalling = false;
bool haveDeployed = false;
bool onGround = false;

const int max_size = 100; //the master array has a MAXIMUM of two sub-arrays
float accelArray[max_size] = {0};
float altArray[max_size] = {0};

int curr_alt_size = 0;
int curr_accel_size = 0;

File smtFile;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while(!Serial) {
    delay(10);
  }
  Serial.println("Serial begin");

  // pinMode(explosiveChargePin, OUTPUT);  // Set parachute pin as output
  // digitalWrite(explosiveChargePin, LOW);
  // Serial.println("LIGHT OFF, INITIALIZING WAIT");
  // delay(10000);
  // digitalWrite(explosiveChargePin, HIGH);
  // Serial.println("LIGHT ON");

  accelSetup();
  altSetup();
  alt();
  accel();

  initAlt = alt();

  Serial.print("Initializing SD card...");
  if (!SD.begin(smtCS)) {
    Serial.println("SPI SMT storage initialization failed");
  } else {
    // Serial.println("SPI SMT storage initialized");
    // smtFile = SD.open("flight_data.txt", FILE_WRITE);
    // if (!smtFile) {
    //   Serial.println("Failed to open file on SMT module");
    // } else {
    //   smtFile.close();
    // }
  }
  Serial.println("initialization done");

  smtFile = SD.open("may_data.txt", FILE_WRITE);
  if (smtFile) {
    Serial.print("Writing to may_data.txt...");
    smtFile.println("testing 1, 2, 3.");
    // close the file:
    smtFile.close();
    Serial.println("done.");
  } else {
    Serial.println("Error opening may_data.txt");
    while(1);
  }

  // LoRa.begin(915E6); 
  // Serial.println("LoRa initialized for transmitting");

  smtFile = SD.open("may18.txt", FILE_WRITE);

}

void loop() {
  delay(50);

  prevAlt = currAlt; //altitude from last round becomes prevAlt
  currAlt = alt(); //new currAlt is collected
  if (currAlt > apogee || currAlt == apogee) { //if currAlt > apogee or currAlt = apogee
    apogee = currAlt; //redefine apogee to currAlt
  } else if (currAlt < apogee && currAlt > (apogee - 5)) { //if currAlt is within five meters of apogee, do nothing
  } else { //otherwise, preliminary fall check is true
    Serial.println("prelimFallCheck = true");
    prelimFallCheck = true;
  }

  AccelDataStructure accelData = accel(); //populate data structure with the return of accel()

  if (curr_alt_size < max_size) { //if array hasn't reached max size
    altArray[curr_alt_size] = currAlt; //populate next empty index with currAlt
    curr_alt_size++;

  } else { //otherwise, if array has reached max size, shift everything to the left one
    for (int i = 0; i < (max_size - 1); i++) {
      altArray[i] = altArray[i+1];
    }
    altArray[max_size - 1] = currAlt;
    
  }

  if (curr_accel_size < max_size) { //if array hasn't reached max size
    accelArray[curr_accel_size] = accelData.z; //populate next empty index with current accel
    curr_accel_size++;
  } else { //otherwise, if array has reached max size, shift everything to the left one
    for (int i = 0; i < (max_size - 1); i++) {
      accelArray[i] = accelArray[i+1];
    }
    accelArray[max_size - 1] = accelData.z;
  }

  if (smtFile) { //if file exists
    smtFile.print(currAlt);
    smtFile.print(",");
    smtFile.println(accelData.z);
    smtFile.flush();
    Serial.println("Wrote to may18.txt");
  } else { //if file doesn't exist
    Serial.println("Error writing to may18.txt");
  }

  // digitalWrite(explosiveChargePin, HIGH);
  // delay(100);
  // digitalWrite(explosiveChargePin, LOW);
  Serial.println(currAlt);
  Serial.println(accelData.z);
  // LoRa.beginPacket(); //transmit data
  // LoRa.print(currAlt);
  // LoRa.print(", ");
  // LoRa.print(accelData.z);
  // LoRa.endPacket();

  if (prelimFallCheck) { //if currAlt is more than five meters below apogee
    Serial.println("prelimFallCheck = true and running trendDown");
    if (trendDown()) { //if last twenty altitude values have trended down
      Serial.println("trendDown = true and falling = true");
      falling = true;
    } else {}
  } else {}

  // if (falling) {
  //   Serial.println("falling = true and running checkFreeFalling");
  //   if (checkFreeFalling()) {
  //     Serial.println("checkFreeFalling = true and doubleCheckFalling = true");
  //     doubleCheckFalling = true;
  //   } else {}

  // }

  if (falling) {
    //PARACHUTE
    Serial.println("Free fall detected! Deploying parachute...");
    smtFile.println("FREE FALL DETECTED -- DEPLOYING PARACHUTE");
    smtFile.flush();
    deployParachute();
  } else {}

  if (falling && (currAlt <= initAlt + 2)) {
    Serial.println("Rocket has landed. Resetting falling to false.");
    smtFile.println("Rocket landed.");
    smtFile.close();
    falling = false;  // Reset falling status
    onGround = true;
    // isOnGround();
    while (true);
  }




}


bool trendDown() {
  float checkingAltArray[11];
  for (int i = 80; i <= 90; i++) { // for these eleven indices in the altitude array
    float groupAvg = 0;
    for (int n = 0; n < 10; n++) { // compute a "moving" average -- (80+81+82...+89) vs (81+82+83...+90) vs(82+83+84...+91), etc
      groupAvg += altArray[i+n];
    }
    groupAvg = groupAvg/10;
    checkingAltArray[i-80] = groupAvg; // insert average into checking array 
  }
  for (int u = 0; u < 10; u++) {
    if (checkingAltArray[u] < checkingAltArray[u+1]) {
      return false;
      break;
    }
  }

  return true;
}

// bool checkFreeFalling() {
//   float errorRange = 0.5;
//   for (int i = 80; i < 100; i++) {
//     if (abs(accelArray[i]) > errorRange) {
//       return false;
//       break;
//     }
//   }
//   return true;

// }

void deployParachute() {
  if (!haveDeployed) { // Ensure parachute is deployed only once
    // digitalWrite(explosiveChargePin, HIGH); // Send signal to deploy parachute
    // delay(500);  // Keep the signal high for 500ms
    // digitalWrite(explosiveChargePin, LOW);  // Turn off signal
    haveDeployed = true;  // Mark parachute as deployed
  }
}

void isOnGround() {
  //write data from SPI SMT to MicroSD card
}
