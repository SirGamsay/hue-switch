#include <Arduino.h>

// ============================================================
//  Button-Test  –  GPIO anpassen falls du einen anderen nutzt
// ============================================================
#define BUTTON_PIN 3    // Taster-Pin → GPIO3 → GND
#define LED_PIN    8    // Onboard-LED am ESP32-C3 Mini (meist GPIO8)
// ============================================================

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("=== Button-Test ===");
    Serial.printf("Warte auf Taster an GPIO%d ...\n", BUTTON_PIN);
}

void loop() {
    bool pressed = (digitalRead(BUTTON_PIN) == LOW);

    digitalWrite(LED_PIN, pressed ? HIGH : LOW);

    static bool last = false;
    if (pressed != last) {
        last = pressed;
        Serial.println(pressed ? "GEDRUECKT" : "losgelassen");
    }
}
