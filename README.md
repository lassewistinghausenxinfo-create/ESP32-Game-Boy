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

## 🔌 Pinout (Wiring)

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
