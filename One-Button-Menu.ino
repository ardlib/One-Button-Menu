// Using SAMD21 board

#include "button.hpp"

OBM btn(PA27);
void buttonCallBack(int state, uint32_t duration_ms) {
  Serial.print(F("CB Current Button State: "));
  Serial.println(state ? F("Pressed") : F("Released"));
  Serial.print(F("CB Duration since Last Event (ms): "));
  Serial.println(duration_ms);
}
void setup() {
  delay(10000); // Startup delay to open Serial Console
  
  Serial.begin(115200);
  
  if (btn.begin()) {
    Serial.println(F("Button Configured"));
  }
  btn.attach(buttonCallBack);
  
  Serial.println(F("Running!"));
}

void loop() {
  // Check if the Buttonb is Detected
  if (btn.detect()) {
    Serial.println(F("Loop Detected Button Press"));
  }
}
