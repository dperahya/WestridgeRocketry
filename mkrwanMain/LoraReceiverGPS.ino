#include <SPI.h>
#include <LoRa.h>


void setup() {
  Serial.begin(9600);
  LoRa.begin(915E6);  // Set frequency to 915 MHz (adjust based on region)
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      String received = LoRa.readString();
      Serial.println(received);
    }
  }
}
