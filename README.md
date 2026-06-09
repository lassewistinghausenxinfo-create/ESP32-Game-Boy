
# 🕹️ ESP32 Game Boy (DIY Retro Console)

A custom-built handheld Game Boy based on the ESP32-WROOM-32D and a 1.8-inch SPI TFT display. 

This project features its own operating system with a main menu and saves high scores permanently on the device flash memory!

## 🎮 Included Games
1. **Flappy Bird:** The classic. Dodge the pipes!
2. **Pong:** Choose between 1-Player mode (vs. PC) or 2-Player mode (1v1 on the same device).
3. **Snake:** Eat the red apples and grow as long as possible.
4. **Space Invaders:** Shoot down the incoming alien waves.

  ## 🛠️ Hardware Requirements
* 1x ESP32 Development Board (e.g., ESP32-WROOM-32D)
* 1x 1.8-inch TFT Display (ST7735 Chip, SPI)
* 6x Push Buttons
* PCB, 7,5 x 10 cm Universal Prototype Board
* Breadboard Jumper Wire Kit
* **Optional (For Portable Power):**
  * 1x 3.7V LiPo Battery
  * 1x TP4056 LiPo Charging Module
  * 1x Small Slide Switch (ON/OFF)

## 🔌 Pinout (Wiring)

**🔋 Battery Power Setup (Optional)**
If you want to make the console truly portable, connect the battery and charger like this:
* LiPo Red Wire (+) -> TP4056 `B+`
* LiPo Black Wire (-) -> TP4056 `B-`
* TP4056 `OUT-` -> ESP32 `GND`
* TP4056 `OUT+` -> To one pin of the Slide Switch
* Slide Switch (other pin) -> ESP32 `5V` (or `VIN`)
*(Note: To charge the device, simply plug a USB cable directly into the TP4056 module!)*

## 🔌 Pinout (Wiring)


https://wokwi.com/projects/466386256192714753

<img width="521.5" height="363" alt="ESP32-Game-Boy" src="https://github.com/user-attachments/assets/39fd7a91-b87c-4aee-bb74-1e946dd2eaad" />


**The TFT Display (Hardware SPI)**
* VCC -> 3.3V or 5V
* GND -> GND
* CS -> GPIO 15
* RES / RST -> GPIO 4
* DC / A0 -> GPIO 2
* SDA / MOSI -> GPIO 23
* SCK / SCL -> GPIO 18
* BL / LED -> 3.3V

**The 6 Buttons (to GND)**
* Button A -> GPIO 13
* Button B -> GPIO 14
* D-Pad UP -> GPIO 27
* D-Pad DOWN -> GPIO 26
* D-Pad LEFT -> GPIO 25
* D-Pad RIGHT -> GPIO 33

## 💻 Software & Libraries
Programmed using the **Arduino IDE**. Requires the following libraries:
* `Adafruit GFX Library`
* `Adafruit ST7735 and ST7789 Library`
* `Preferences` (for high score saving)

Im README nach "Libraries" diese Sektion einfügen:

## 🚀 Getting Started
1. Arduino IDE installieren
2. ESP32 Board-Support installieren:
  → Datei → Einstellungen → Boards-URL:
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
3. Board wählen: "ESP32 Dev Module"
4. Partition Scheme: "Default 4MB with spiffs"
5. Die 3 Libraries installieren (Sketch → Bibliothek einbinden)
6. ESP32-Game-Boy.ino öffnen & hochladen
