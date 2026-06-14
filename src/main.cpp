#include <Arduino.h>
#include <NimBLEDevice.h>

// ============================================================
//  KONFIGURATION
// ============================================================
#define HUE_MAC    "da:0d:03:84:73:0d"
#define BUTTON_PIN 6
#define LED_EXT    4   // externe LED: GPIO4 → 220Ω → LED → GND  (active HIGH)
#define LED_INT    8   // onboard LED (active LOW, Debugging)
#define FAKE_GND   2   // kein freier GND-Pin → GPIO2 als GND-Ersatz (OUTPUT LOW)
// ============================================================

static NimBLEUUID svcUUID("932c32bd-0000-47a2-835a-a8d455b859dd");
static NimBLEUUID pwrUUID("932c32bd-0002-47a2-835a-a8d455b859dd");

static void ledOn() {
    digitalWrite(LED_EXT, HIGH);
    digitalWrite(LED_INT, LOW);
}
static void ledOff() {
    digitalWrite(LED_EXT, LOW);
    digitalWrite(LED_INT, HIGH);
}
static void ledBlink(int n, int ms = 80) {
    for (int i = 0; i < n; i++) {
        ledOn();  delay(ms);
        ledOff(); delay(ms);
    }
}

class SecurityCB : public NimBLESecurityCallbacks {
    void onAuthenticationComplete(ble_gap_conn_desc* desc) override {}
    bool onConfirmPIN(uint32_t pin) override { return true; }
    uint32_t onPassKeyRequest() override { return 0; }
    void onPassKeyNotify(uint32_t pass_key) override {}
    bool onSecurityRequest() override { return true; }
};

void toggleLight() {
    NimBLEAddress addr(HUE_MAC, BLE_ADDR_RANDOM);
    NimBLEClient* client = NimBLEDevice::createClient();
    ledOn();

    if (!client->connect(addr)) {
        NimBLEDevice::deleteClient(client);
        ledBlink(5, 60);
        return;
    }

    delay(200);
    if (!client->secureConnection()) {
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        ledBlink(5, 60);
        return;
    }

    NimBLERemoteService* svc = client->getService(svcUUID);
    if (!svc) {
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        ledBlink(5, 60);
        return;
    }

    NimBLERemoteCharacteristic* pwr = svc->getCharacteristic(pwrUUID);
    if (!pwr) {
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        ledBlink(5, 60);
        return;
    }

    uint8_t current = 0;
    std::string raw = pwr->readValue();
    if (!raw.empty()) current = static_cast<uint8_t>(raw[0]);

    uint8_t next = current ? 0x00 : 0x01;
    bool written = pwr->writeValue(&next, 1, false);

    client->disconnect();
    NimBLEDevice::deleteClient(client);
    delay(1000);

    written ? ledBlink(3, 100) : ledBlink(5, 60);
}

static bool lastBtn        = HIGH;
static uint32_t lastChange = 0;

void setup() {
    pinMode(FAKE_GND, OUTPUT);
    digitalWrite(FAKE_GND, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_EXT, OUTPUT);
    pinMode(LED_INT, OUTPUT);
    ledOff();

    NimBLEDevice::init("HueSwitch");
    NimBLEDevice::setPower(ESP_PWR_LVL_P3);
    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityCallbacks(new SecurityCB());

    ledBlink(2, 150);  // Startbestätigung
}

void loop() {
    bool btn = digitalRead(BUTTON_PIN);
    uint32_t now = millis();

    if (btn != lastBtn && (now - lastChange) > 50) {
        lastChange = now;
        lastBtn    = btn;
        if (btn == LOW) toggleLight();
    }
    delay(10);
}
