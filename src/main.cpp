#include <Arduino.h>
#include "sensesp.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_put_request_listener.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp/signalk/signalk_metadata.h"
#include "sensesp_app.h"
#include "sensesp_app_builder.h"

using namespace sensesp;

// --- Gerätekonfiguration ---
const char* hostname = "ESP32-lightcontrol"; // Hostname des Geräts

// --- Signal K Pfad ---
// Achtung: Passe diesen Pfad an dein Signal K System an, bevor du kompilierst.
// Beispiel: "electrical.lights.saloon.forward.state"
const char* sk_path_relay_state = "electrical.lights.<saloon>.<forward>.state";

// --- Pin-Konfiguration ---
const int relayPin = 26;   // Relais an Pin 26
const int buttonPin = 33;  // Taster an Pin 33
const int ledPin = 18;     // LED am Taster (GPIO18)

// --- Zustandsvariablen ---
bool relayState = false;        // aktueller Relaiszustand
bool lastButtonState = HIGH;    // letzter Tasterzustand
bool buttonState = HIGH;        // aktueller Tasterzustand
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Entprellzeit

// --- Signal K Variablen ---
SKOutput<bool>* relay_sk_output = nullptr;
SKPutRequestListener<bool>* relay_sk_listener = nullptr;

// --- Funktion zum Setzen des Relaiszustands ---
void setRelayState(bool newState, bool fromSK = false) {
  relayState = newState;

  // LOW-aktiv: Relais schaltet bei LOW ein
  digitalWrite(relayPin, relayState ? LOW : HIGH);

  // LED soll leuchten, wenn das Relais AUS ist
  digitalWrite(ledPin, relayState ? LOW : HIGH);

  // Signal K Output nur aktualisieren, wenn lokal geschaltet wurde
  if (!fromSK && relay_sk_output != nullptr) {
    relay_sk_output->set(relayState);
  }

  Serial.println(String("Relais ") + (relayState ? "EIN" : "AUS") +
               (fromSK ? " (über Signal K)" : " (lokal)"));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starte Lichtsteuerung Heckkajüte...");

  // Warnung bei unveränderten Platzhaltern im Signal K-Pfad
  if (String(sk_path_relay_state).indexOf('<') != -1 || String(sk_path_relay_state).indexOf('>') != -1) {
    Serial.println("WARNUNG: Signal K-Pfad enthält Platzhalter (<...>). Bitte 'sk_path_relay_state' in main.cpp anpassen!");
  }

  // --- Pin Setup ---
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Relais initial AUS
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  // LED initial entsprechend aktuellem Relaiszustand setzen (LED an, wenn Relais AUS)
  digitalWrite(ledPin, relayState ? LOW : HIGH);

  // --- SensESP Setup ---
  SetupLogging();
  SensESPAppBuilder builder;
  sensesp_app = builder.set_hostname(hostname)->get_app();

  // --- Signal K Output ---
  relay_sk_output = new SKOutput<bool>(sk_path_relay_state);

  // Nach Verbindungsaufbau aktuellen Zustand einmal senden
  event_loop()->onRepeat(500, []() {
    static bool pushed_initial = false;
    if (!pushed_initial &&
        sensesp_app->get_ws_client()->get_connection_status() == "Connected") {
      relay_sk_output->set(relayState);
      pushed_initial = true;
      return false;  // Timer stoppen
    }
    return true;
  });

  // --- Signal K Listener (PUT Request Handling) ---
  relay_sk_listener = new SKPutRequestListener<bool>(sk_path_relay_state);
  auto relayConsumer = new LambdaConsumer<bool>([](bool value) {
    setRelayState(value, true);
  });
  relay_sk_listener->connect_to(relayConsumer);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        setRelayState(!relayState, false);
      }
    }
  }

  lastButtonState = reading;
  event_loop()->tick();
}
