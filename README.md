# ESP32 Signal K Light Controller

This project controls a **relay** for **light switching** using an **ESP32** (e.g. on a boat) and integrates with a **[Signal K](https://signalk.org)** network via **SenseESP**.

The light can be switched **locally by a push button** or **remotely via Signal K / Node-RED / KIP**.


---

## ⚙️ Features

- Controls a **LOW-active relay** with an ESP32
- Supports **push-button input** with debouncing
- Shows the current state via an **LED** (LED lights when the light is **off**)
- Synchronizes state bidirectionally with **Signal K**:
  - processes `PUT` requests from Signal K or Node-RED
  - regularly reports current state (`true` / `false`) to Signal K
- Automatic initial synchronization on connection

---

## 🧠 Signal K Path

The Signal K path used by default is:

**electrical.lights.<cabin>.<aft>.state**

(Replace `<cabin>` and `<aft>` with names used in your Signal K system.)

**Important:** Before compiling, adjust the `sk_path_relay_state` constant in `main.cpp` to match your own naming. Replace the placeholders (`<cabin>`, `<aft>`) or the entire path with the Signal K path you use (e.g. `electrical.lights.saloon.forward.state`). This change must be made before compiling and uploading to the ESP.

At startup the program checks the path for placeholders and prints a **warning on the serial console** if placeholders are still present (e.g. `<cabin>`). Please change the path until no warning appears.

This makes the function clearly identifiable as a **cabin light**, regardless of whether it is implemented with a relay, MOSFET or other hardware.

---

## 🔌 Hardware Setup

| Component | Description | Pin |
|-------------|--------------|-----|
| Relay module | LOW-active, switches light or LED string | GPIO 26 |
| Push button | with internal pull-up | GPIO 33 |
| LED | Status indicator, lights when light is **off** | GPIO 18 |
| ESP32 | DevKitC / D1 Mini compatible | – |

### 🔧 Hardware I used

- **Push button:** https://amzn.to/48JyK2P
- **Relay module:** https://amzn.to/3L4kEPJ
- **ESP32 D1 Mini:** https://amzn.to/4hpdCBi
- **JST 2.0 PH 4-pin connector:** https://amzn.to/4aoSdXz
- **JST 2.0 PH 3-pin connector:** https://amzn.to/48QAc1O

---

## 📶 Network and App Configuration

In `main.cpp`:

```cpp
const char* hostname = "ESP32-lightcontrol"; // device hostname
// WiFi and Signal K configuration are handled automatically by SenseESP.
// On first boot the device opens a WiFi access point for setup (default password: thisisfine).
// Use the web UI to connect the ESP32 to your Signal K server.
```

🧱 Node-RED Integration

To control the light via Node-RED you need these nodes:

| Node | Description |
|------|-------------|
| **inject** | manually emits a value (e.g. `true` / `false`) |
| **function** *(optional)* | transforms or adds logic |
| **Signal K PUT node** *(from [node-red-contrib-signalk](https://flows.nodered.org/node/node-red-contrib-signalk))* | sends the value to the Signal K server |
| **debug** *(optional)* | shows messages for debugging |

Quick example:

Install the package:
```bash
npm install node-red-contrib-signalk
```

Create a flow like this:

- **inject node:** payload true or false (Boolean)
- **Signal K PUT node:** Path: `electrical.lights.<cabin>.<aft>.state`  Target: self  Server: your Signal K server

Connect: inject → Signal K PUT → debug

This lets you switch the light using Node-RED. The current state (`true` / `false`) is also automatically reported to Signal K by SenseESP for dashboards or automations.

---

## 📁 Additional project files

The project also includes the following files/folders:

- `Gerber/` — Gerber files for PCB manufacturing
- `Case 3mf/` — 3MF file for the 3D-printable enclosure
- `NodeRed Flow/` — Node-RED flow to use as a digital switch in Signal K

These files make it easy to have the PCB manufactured, print the enclosure, and quickly integrate the Node-RED flow.

---

## 🧩 Code structure
Main functions:

- `setRelayState(bool newState, bool fromSK)` — switches relay and LED, updates Signal K
- `SKOutput<bool>` — reports current state to Signal K
- `SKPutRequestListener<bool>` — handles external PUT requests
- `LambdaConsumer<bool>` — connects the listener to the relay control logic

---

## 🛠️ Build & Upload
Requirements:
- PlatformIO
- Library: SenseESP

Build and upload:
```bash
pio run --target upload
```
After upload the ESP32 starts automatically and shows messages on the serial monitor.

---

⚡ License
![license](https://img.shields.io/badge/license-GPLv3-blue) ![license-hw](https://img.shields.io/badge/hardware-CERN%20OHL--S%20v2-orange)
- Software — GPLv3 (see `LICENSE`); 
- Hardware — CERN OHL-S v2 (see `LICENSE-HARDWARE`).
