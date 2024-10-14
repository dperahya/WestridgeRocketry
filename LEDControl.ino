const int A = 1;
const int B = 2;
const int C = 3;

// Delay for multiplexing (in microseconds)
const int multiplexDelay = 1000;
const int blinkDelay = 750;
const int pulseDelay = 150;  // Delay between pulses
const int pulseCycleDelay = 2000;

unsigned long previousBlink = 0;
unsigned long lastPulseTime[6] = {0, 0, 0, 0, 0, 0};  // Tracks the last pulse time for each LED
unsigned long previousPulseCycle[6] = {0, 0, 0, 0, 0, 0};  // Tracks the last cycle time for each LED
int pulseCounts[6] = {0, 0, 0, 0, 0, 0};  // Tracks the number of pulses for each LED in the current cycle
int pulseTargets[6] = {1, 1, 1, 1, 1, 1};

// Array to hold the state of each LED
int ledModes[6] = {OFF, OFF, OFF, OFF, OFF, OFF};
bool ledStates[6] = {false, false, false, false, false, false};

void controlLEDs() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlink > blinkDelay) {
    previousBlink = currentMillis;
    for (int i = 0; i < 6; i++) {
      if (ledModes[i] == BLINK) {
        ledStates[i] = !ledStates[i];
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    if (ledModes[i] == PULSE) {
      int pulseTarget = pulseTargets[i];

      if (currentMillis - previousPulseCycle[i] > pulseCycleDelay) {
        pulseCounts[i] = 0;  // Reset pulse count for new cycle
        previousPulseCycle[i] = currentMillis;
      }

      if (pulseCounts[i] < pulseTarget && (currentMillis - lastPulseTime[i] > pulseDelay)) {
        ledStates[i] = !ledStates[i];  // Toggle LED state for pulsing
        lastPulseTime[i] = currentMillis;

        if (!ledStates[i]) {
          pulseCounts[i]++;  // Increment count only after the LED turns off
        }
      }
    }

    // switch to see which LEDs turn on
    switch (ledModes[i]) {
      case STEADY:
        turnOnLED(i);  // Turn on the LED
        break;
      case BLINK:
      case PULSE:
        if (ledStates[i]) {
          turnOnLED(i);
        }
        break;
    }
    delayMicroseconds(multiplexDelay);  // Short delay for multiplexing
    turnOffLEDs();
  }
}

void turnOnLED(int ledNumber) {
  switch (ledNumber) {
    case 0:
      pinMode(A, OUTPUT); pinMode(B, OUTPUT); pinMode(C, INPUT);
      digitalWrite(A, HIGH); digitalWrite(B, LOW);
      break;
    case 1:
      pinMode(A, OUTPUT); pinMode(B, OUTPUT); pinMode(C, INPUT);
      digitalWrite(A, LOW); digitalWrite(B, HIGH);
      break;
    case 2:
      pinMode(A, INPUT); pinMode(B, OUTPUT); pinMode(C, OUTPUT);
      digitalWrite(B, HIGH); digitalWrite(C, LOW);
      break;
    case 3:
      pinMode(A, INPUT); pinMode(B, OUTPUT); pinMode(C, OUTPUT);
      digitalWrite(B, LOW); digitalWrite(C, HIGH);
      break;
    case 4:
      pinMode(A, OUTPUT); pinMode(B, INPUT); pinMode(C, OUTPUT);
      digitalWrite(A, HIGH); digitalWrite(C, LOW);
      break;
    case 5:
      pinMode(A, OUTPUT); pinMode(B, INPUT); pinMode(C, OUTPUT);
      digitalWrite(A, LOW); digitalWrite(C, HIGH);
      break;
  }
}

void turnOffLEDs() {
  pinMode(A, INPUT); pinMode(B, INPUT); pinMode(C, INPUT);
}

void setLEDMode(int i, ledMode LEDMode, int pulseTarget) {
  Serial.println(i);
  Serial.println(LEDMode);
  Serial.println(pulseTarget);
  ledModes[i] = LEDMode;
  pulseTargets[i] = pulseTarget;
}

