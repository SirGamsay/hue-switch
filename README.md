# Hue Switch

A minimal Bluetooth (BLE) push-button that toggles a Philips Hue lamp — built around an **ESP32-C3** and [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).

No bridge, no Wi-Fi, no cloud. The microcontroller talks directly to the lamp's built-in BLE radio.

## How it works

Pressing the button triggers a BLE connection to the Hue lamp. The firmware reads the current on/off state from the lamp's GATT characteristic and writes the opposite value back.

- 2 blinks on startup — ready
- LED solid during connection attempt
- 3 blinks — toggled successfully
- 5 fast blinks — connection or write failed

## Hardware

| Part | Notes |
|---|---|
| ESP32-C3-DevKitM-1 | any ESP32-C3 board works |
| Momentary push button | wired between GPIO6 and GND |
| LED + 220 Ω resistor | GPIO4 → resistor → LED → GND |
| Philips Hue bulb | must support BLE (gen 3+, firmware ≥ 1.50) |

**GPIO overview:**

| GPIO | Function |
|---|---|
| 2 | Fake GND (output LOW — frees a GND pad) |
| 4 | External LED (active HIGH) |
| 6 | Button (INPUT\_PULLUP) |
| 8 | Onboard LED for debugging (active LOW) |

## Enclosure

The `cad/` directory contains FreeCAD source files and STL exports for a small printed enclosure.

## Configuration

Open `src/main.cpp` and adjust the defines at the top:

```cpp
#define HUE_MAC    "xx:xx:xx:xx:xx:xx"  // BLE MAC of your Hue lamp
#define BUTTON_PIN 6
#define LED_EXT    4
#define LED_INT    8
#define FAKE_GND   2
```

To find your lamp's MAC, flash the `ble-scan` environment (see below) and look for a device whose name contains "Hue".

## Build & Flash

Requires [PlatformIO](https://platformio.org/).

```bash
# Flash the main firmware
pio run -e hue-controller -t upload

# Scan for BLE devices (to find your lamp's MAC)
pio run -e ble-scan -t upload && pio device monitor

# Test the button wiring
pio run -e button-test -t upload && pio device monitor
```

## First pairing

The ESP32-C3 bonds with the lamp on the first connection using "just works" BLE pairing (no PIN). The bond is stored in NVS flash and reused on subsequent connections. If pairing fails, erase the flash once with:

```bash
pio run -e hue-controller -t erase
```

## License

MIT
