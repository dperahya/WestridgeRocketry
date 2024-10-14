enum ledMode {
  OFF,
  STEADY,
  BLINK,
  PULSE
};

void setLEDMode(int i, ledMode LEDMode, int pulseTarget=1);
void controlLEDs();