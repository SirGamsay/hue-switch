#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("serial ok");
    delay(1000);
}
