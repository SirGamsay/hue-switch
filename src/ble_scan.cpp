#include <Arduino.h>
#include <NimBLEDevice.h>

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== BLE Scan ===");

    NimBLEDevice::init("");
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);

    Serial.println("Scanne 10 Sekunden …");
    NimBLEScanResults results = scan->start(10, false);

    Serial.printf("\n%d Gerät(e) gefunden:\n", results.getCount());
    for (int i = 0; i < results.getCount(); i++) {
        NimBLEAdvertisedDevice dev = results.getDevice(i);
        Serial.printf("  [%d] %s  RSSI: %d  Name: %s\n",
            i,
            dev.getAddress().toString().c_str(),
            dev.getRSSI(),
            dev.getName().c_str());
    }
    Serial.println("\nFertig. Reset für neuen Scan.");
}

void loop() {}
