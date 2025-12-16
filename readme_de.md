# ESP32 Signal K Lichtsteuerung

Dieses Projekt steuert über einen **ESP32** ein Relais zur **Lichtsteuerung** (z. B. auf einem Boot)  
und integriert sich vollständig in ein **[Signal K](https://signalk.org)** Netzwerk über **SenseESP**.

Das Licht kann sowohl **lokal über einen Taster**, als auch **remote über Signal K / Node-RED / KIP** geschaltet werden.

---

## ⚙️ Funktionen

- Steuert ein **Relais (LOW-aktiv)** über einen ESP32  
- Unterstützt **Tastereingabe** mit Entprellung  
- Zeigt den aktuellen Zustand über eine **LED** an (LED leuchtet, wenn Licht **aus** ist)  
- Synchronisiert Zustand bidirektional mit **Signal K**:
  - `PUT`-Requests von Signal K oder Node-RED werden verarbeitet  
  - Aktueller Zustand (`true` / `false`) wird regelmäßig an Signal K gesendet  
- Automatische Initial-Synchronisation beim Verbindungsaufbau

---

## 🧠 Signal K Pfad

Der verwendete Signal K-Pfad lautet:

**electrical.lights.cabin.aft.state**

**Wichtig:** Passe vor dem Kompilieren in `main.cpp` die Konstante `sk_path_relay_state` entsprechend deinen eigenen Bezeichnungen an. Ersetze die Platzhalter (`<cabin>`, `<aft>`) oder den gesamten Pfad durch den für dein System gewünschten Signal K-Pfad (z. B. `electrical.lights.saloon.forward.state`). Diese Änderung muss erfolgen, bevor du das Programm kompilierst und auf den ESP aufspielst.
Beim Start prüft das Programm `main.cpp` den Pfad auf Platzhalter und gibt eine **Warnung auf der seriellen Konsole** aus, wenn noch Platzhalter vorhanden sind (z. B. `<cabin>`). Bitte passe den Pfad an, bis keine Warnung mehr erscheint.
Damit ist die Funktion eindeutig als **Kabinenlicht** definiert,  
unabhängig davon, ob sie über Relais, MOSFET oder andere Hardware umgesetzt wird.

---

## 🔌 Hardware-Setup

| Komponente | Beschreibung | Pin |
|-------------|--------------|-----|
| Relaismodul | LOW-aktiv, schaltet Licht oder LED-Kreis | GPIO 26 |
| Taster | mit internem Pull-Up | GPIO 33 |
| LED | Statusanzeige, leuchtet wenn Licht **aus** | GPIO 18 |
| ESP32 | DevKitC / D1 Mini kompatibel | – |

### 🔧 Von mir verwendete Hardware

- **Taster:** [https://amzn.to/48JyK2P](https://amzn.to/48JyK2P)  
- **Relaismodul:** [https://amzn.to/3L4kEPJ](https://amzn.to/3L4kEPJ)  
- **ESP32 D1 Mini:** [https://amzn.to/4hpdCBi](https://amzn.to/4hpdCBi)
- **JST 2.0 PH 4 Pin Steckverbinder:** [https://amzn.to/4aoSdXz](https://amzn.to/4aoSdXz)
- **JST 2.0 PH 3 Pin Steckverbinder** [https://amzn.to/48QAc1O](https://amzn.to/48QAc1O)

---

## 📶 Netzwerk- und App-Konfiguration

In der Datei `main.cpp`:

```cpp
const char* hostname = "ESP32-lightcontrol"; // Hostname im Netzwerk
WLAN- und Signal K-Konfiguration werden automatisch über SenseESP abgewickelt.
Nach dem ersten Start öffnet das Gerät ein WLAN-Access-Point mit dem Hostnamen zur Einrichtung (Standard WLAN Passwort: thisisfine).
Über das WebUI kann der der ESP32 mit dem SignalK Netzwerk verbunden werden. 
```

🧱 Node-RED Integration

Um das Licht über Node-RED zu schalten, benötigst du folgende Nodes:

| Node                                                                                                                        | Beschreibung                                        |
| --------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------- |
| **inject**                                                                                                                  | Erzeugt manuell ein Signal (z. B. `true` / `false`) |
| **function** *(optional)*                                                                                                   | Kann zur Umwandlung oder Logiksteuerung dienen      |
| **Signal K PUT node** *(aus dem [node-red-contrib-signalk](https://flows.nodered.org/node/node-red-contrib-signalk) Paket)* | Sendet den Wert an den Signal K-Server              |
| **debug** *(optional)*                                                                                                      | Zum Anzeigen von Statusmeldungen oder Rückmeldungen |

**Beispiel Flow vereinfacht**

Installiere das Paket:
```cpp
npm install node-red-contrib-signalk
```

Erstelle folgenden Flow:

- **inject node:**
Payload: true oder false
Typ: Boolean

**Signal K PUT node:**
Pfad: electrical.lights.cabin.aft.state

**Ziel:** self
**Server:** dein Signal K-Server

Verbinde inject → SignalK PUT → debug

→ Damit kannst du direkt über Node-RED das Licht ein- und ausschalten.
Der aktuelle Zustand (true / false) wird außerdem automatisch von SenseESP an Signal K gemeldet und steht dort für Dashboards oder Automatisierungen zur Verfügung.


📁 Zusätzliche Projektdateien

Im Projekt sind folgende zusätzliche Dateien/Ordner enthalten:

- `Gerber/` — Gerber-Dateien für die Platine (Herstellung / Fertigung).
- `Case 3mf/` — 3MF-Datei für das Gehäuse zum 3D-Druck.
- `NodeRed Flow/` — Node-RED Flow zum Einsatz als Digital Switch in Signal K.

Diese Dateien ermöglichen die einfache Fertigung der Platine, den 3D-Druck des Gehäuses und eine sofort einsatzbereite Node-RED-Integration.


🧩 Aufbau des Codes
Hauptfunktionen:
setRelayState(bool newState, bool fromSK)
→ Schaltet Relais, LED und aktualisiert Signal K

SKOutput<bool>
→ Gibt den aktuellen Zustand an Signal K aus

SKPutRequestListener<bool>
→ Reagiert auf externe Steuerbefehle (PUT-Requests)

LambdaConsumer<bool>
→ Verbindet den Listener mit der Relay-Steuerlogik

🛠️ Kompilieren und Flashen
Voraussetzungen:
PlatformIO

Bibliothek: SenseESP

Befehl:

pio run --target upload
Nach dem Upload startet der ESP32 automatisch und zeigt im Serial Monitor:


Taster-Toggle für Relais (LOW-aktiv)...

⚡ Lizenz
![license](https://img.shields.io/badge/license-GPLv3-blue) ![license-hw](https://img.shields.io/badge/hardware-CERN%20OHL--S%20v2-orange)
Software — GPLv3 (see `LICENSE`); 
Hardware — CERN OHL-S v2 (see `LICENSE-HARDWARE`).