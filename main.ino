if (!Blynk.connected()) {
    digitalWrite(WIFI_LED_PIN, LOW);   // OFF
    Serial.println(F("[Blynk] Not connected, trying..."));
    Blynk.connect(BLYNK_CONNECT_MS);   // time-limited attempt
  } else {
    digitalWrite(WIFI_LED_PIN, HIGH);  // ON
    Serial.println(F("[Blynk] Connected"));
  }
}

// ============ Blynk Events ============

BLYNK_CONNECTED() {
  Serial.println(F("[Blynk] Connected -> sending relay states"));
  Blynk.virtualWrite(VPIN_RELAY1, relay1State);
  Blynk.virtualWrite(VPIN_RELAY2, relay2State);
}

BLYNK_WRITE(VPIN_RELAY1) {
  bool newState = (param.asInt() == 1);
  Serial.print(F("[Blynk] V1 received: "));
  Serial.println(newState);
  setRelay1(newState, "Blynk");
}

BLYNK_WRITE(VPIN_RELAY2) {
  bool newState = (param.asInt() == 1);
  Serial.print(F("[Blynk] V2 received: "));
  Serial.println(newState);
  setRelay2(newState, "Blynk");
}

// ============ Latched Switch Handling ============
//   Switch to GND (LOW) -> Relay ON
//   Switch open  (HIGH) -> Relay OFF

void handleSwitches() {
  unsigned long now = millis();

  // ---- Switch 1 ----
  bool r1 = digitalRead(SWITCH1_PIN);
  if (r1 != rawSwitch1) {
    rawSwitch1 = r1;
    changeTime1 = now;
  }
  if ((now - changeTime1) >= DEBOUNCE_MS && rawSwitch1 != stableSwitch1) {
    stableSwitch1 = rawSwitch1;
    Serial.print(F("[SWITCH] Switch1 = "));
    Serial.println(stableSwitch1);
    setRelay1(stableSwitch1 == LOW, "Switch1");
  }

  // ---- Switch 2 ----
  bool r2 = digitalRead(SWITCH2_PIN);
  if (r2 != rawSwitch2) {
    rawSwitch2 = r2;
    changeTime2 = now;
  }
  if ((now - changeTime2) >= DEBOUNCE_MS && rawSwitch2 != stableSwitch2) {
    stableSwitch2 = rawSwitch2;
    Serial.print(F("[SWITCH] Switch2 = "));
    Serial.println(stableSwitch2);
    setRelay2(stableSwitch2 == LOW, "Switch2");
  }
}

// ============ Setup ============

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println();
  Serial.println(F("===== ESP8266 2-Relay Blynk + Latched Switch + EEPROM (v3) ====="));

  // EEPROM init + load FIRST (before touching relays)
  EEPROM.begin(512);
  loadRelayStatesFromEEPROM();

  // Switch inputs
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  delay(20);  // let pull-ups settle

  stableSwitch1 = rawSwitch1 = digitalRead(SWITCH1_PIN);
  stableSwitch2 = rawSwitch2 = digitalRead(SWITCH2_PIN);
  Serial.print(F("[SWITCH] Initial S1="));
  Serial.print(stableSwitch1);
  Serial.print(F(" S2="));
  Serial.println(stableSwitch2);

  // Optional: a switch that is ON at boot wins
  if (BOOT_SWITCH_PRIORITY) {
    if (stableSwitch1 == LOW) relay1State = true;
    if (stableSwitch2 == LOW) relay2State = true;
  }

  // Set the relay level BEFORE making the pin an output (no glitch)
  digitalWrite(RELAY1_PIN, relay1State ? RELAY_ON : RELAY_OFF);
  digitalWrite(RELAY2_PIN, relay2State ? RELAY_ON : RELAY_OFF);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  if (BOOT_SWITCH_PRIORITY) saveRelayStatesToEEPROM();  // keep memory in sync
  applyRelayStates();

  // WiFi LED
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, LOW);

  // WiFi + Blynk
  activeSsid = ssid;
  Serial.print(F("[WiFi] Connecting to '"));
  Serial.print(activeSsid);
  Serial.println(F("' ..."));
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(activeSsid.c_str(), pass);

  Blynk.config(auth);   // no blocking connect here

  timer.setInterval(STATUS_INTERVAL_MS, checkBlynkStatus);

  Serial.println(F("[SETUP] Completed"));
}

// ============ Loop ============

void loop() {
  // Manual switches ALWAYS work, regardless of WiFi/Blynk state
  handleSwitches();

  // Run Blynk only while already connected, so it can never block the
  // switches. Reconnects are handled by checkBlynkStatus() via the timer.
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
  }

  timer.run();
}
