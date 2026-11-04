# WiFi Smart Appliance Controller with Power Status Feedback

A WiFi-enabled controller built on the **ESP8266** that switches two AC appliances ON and OFF through the **Blynk** mobile app, with physical latched switches as a manual override. The current relay state is reported back to the app, and the last state is restored after a power cut.

## Features

- **Remote control:** two relay channels controlled from the Blynk app (virtual pins `V1` and `V2`).
- **Power status feedback:** relay states are sent to the app whenever the device connects, so the app always shows the real ON/OFF status of each appliance.
- **Manual override:** two latched wall-style switches control the relays directly. They keep working even when WiFi or Blynk is down.
- **Switch debouncing:** software debounce for stable, flicker-free switch readings.
- **State retention:** relay states are saved to EEPROM and restored after power loss or reboot.
- **Boot switch priority (optional):** a switch that is ON at boot takes priority over the saved state.
- **Glitch-free startup:** the relay output level is set before the pin is configured as an output, so appliances do not briefly switch on at boot.
- **Non-blocking connectivity:** Blynk runs only while connected and reconnection is handled by a timer, so network problems never block the switches.
- **Connection indicator:** a status LED shows whether the device is connected to Blynk.

## Hardware

| Component | Purpose |
|---|---|
| NodeMCU (ESP8266) | Main controller and WiFi |
| 2-channel 5V relay module (optocoupler isolated) | Switches the AC appliances |
| 2 latched switches | Manual ON/OFF control |
| LED with 220 Ω resistor | WiFi / Blynk connection indicator |
| HLK-5M05 (230V AC to 5V DC) | Compact power supply for the board and relays |

## Software

- Arduino IDE with ESP8266 board support
- Blynk library
- ESP8266WiFi and EEPROM libraries (included with the ESP8266 core)

## How it works

1. A command from the Blynk app (`V1` / `V2`) reaches the ESP8266 over WiFi.
2. The ESP8266 switches the matching relay and saves the new state to EEPROM.
3. The new state is written back to the app, so the displayed status matches the real relay state.
4. A physical switch can change the same relay at any time, and the app is updated to match.

## Setup

1. Create a Blynk template with two switch widgets on `V1` and `V2`.
2. Open the `.ino` file and set your credentials:
   ```cpp
   char auth[] = "YOUR_BLYNK_TOKEN";
   char ssid[] = "YOUR_WIFI_NAME";
   char pass[] = "YOUR_WIFI_PASSWORD";
   ```
3. Select your ESP8266 board and port in the Arduino IDE, then upload.
4. Open the Serial Monitor at **9600 baud** to see connection and relay logs.

## Circuit

Add photos of your own build here.

Pin mapping: relays on `D5` / `D6`, manual switches on `D1` / `D2`, status LED on `D0`.

The wiring is based on the NodeMCU smart switch circuit from TechStudyCell / iotcircuithub.com. The firmware adds EEPROM state retention, non-blocking Blynk handling, boot switch priority and switch debouncing.

## Safety

This project switches mains voltage. Use a properly rated relay module, keep the high-voltage side insulated and enclosed, and never touch the circuit while it is powered.

## Author

**Govinda Kumar**
B.Tech ECE, Haldia Institute of Technology
GitHub: [CODER-G45](https://github.com/CODER-G45)
