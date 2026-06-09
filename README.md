
# 🕹️ ESP32 Game Boy (DIY Retro Console)

A custom-built handheld Game Boy based on the ESP32-WROOM-32D and a 1.8-inch SPI TFT display. 

This project features its own operating system with a main menu and saves high scores permanently on the device flash memory!

<img width="170" height="226" alt="Bild 3" src="https://github.com/user-attachments/assets/ebc8f6ed-6f4a-4e77-ab57-67abe0bc0589" />
<img width="170" height="226"  alt="Bild 2" src="https://github.com/user-attachments/assets/e42d420d-58f8-4fb8-83d4-eb1bbdf21316" />
<img width="170" height="226"  alt="Bild 1" src="https://github.com/user-attachments/assets/64556ac1-65b6-41b3-9f31-79c9fe71f475" />


## 🎮 Included Games
1. **Flappy Bird:** The classic. Dodge the pipes!
2. **Pong:** Choose between 1-Player mode (vs. PC) or 2-Player mode (1v1 on the same device).
3. **Snake:** Eat the red apples and grow as long as possible.
4. **Space Invaders:** Shoot down the incoming alien waves.
5. **TETRIS:**
6. **Breakout:**

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

## 🚀 Getting Started

1. Install the Arduino IDE.
2. Install the ESP32 board support:
   → File → Preferences → Additional Boards Manager URLs:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Select the board: **"ESP32 Dev Module"**.
4. Set the Partition Scheme to: **"Default 4MB with spiffs"**.
5. Install the 3 required libraries (Sketch → Include Library → Manage Libraries).
6. Open the `ESP32-Game-Boy.ino` file and hit upload!
