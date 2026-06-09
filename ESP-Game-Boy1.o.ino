#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Preferences.h>

// --- Display Pins ---
#define TFT_CS    15
#define TFT_RST   4
#define TFT_DC    2
#define TFT_SDA   23
#define TFT_SCK   18

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_SDA, TFT_SCK, TFT_RST);
Preferences preferences;

// --- Button Pins ---
#define BTN_A     13
#define BTN_B     14
#define BTN_UP    27
#define BTN_DOWN  26
#define BTN_LEFT  25
#define BTN_RIGHT 33

// --- Debounce Helfer ---
bool btnA_pressed = false, btnB_pressed = false;
bool btnUp_pressed = false, btnDown_pressed = false;
bool btnLeft_pressed = false, btnRight_pressed = false;

#define BTN(pin) (digitalRead(pin) == LOW)

// --- Spielzustände ---
int gameState = 0;
int menuSelection = 0;
const int totalGames = 6;
const char* gameNames[6] = {
  "1. Flappy Bird",
  "2. Pong",
  "3. Snake",
  "4. Space Invaders",
  "5. Tetris",
  "6. Breakout"
};

// ==========================================
// GLOBALE HIGHSCORES
// ==========================================
int highscoreFlappy = 0;
int highscoreSnake   = 0;
int highscoreTetris  = 0;
int highscoreInvaders = 0;

// ==========================================
// FLAPPY BIRD
// ==========================================
float birdY, birdVelocity;
const float gravity = 0.55, jumpForce = -4.2;
int pipeX, pipeGapY, pipeGapSize = 42, pipeWidth = 14;
int flappyScore;
bool flappyOver;

void resetFlappyBird() {
  birdY = 60; birdVelocity = 0;
  pipeX = 160; pipeGapY = 45;
  flappyScore = 0; flappyOver = false;
  btnA_pressed = false;
  tft.fillScreen(ST77XX_CYAN);
  tft.setTextColor(ST77XX_BLACK); tft.setTextSize(1);
  tft.setCursor(60, 2); tft.print("B=Menu");
}

void runFlappyBird() {
  if (BTN(BTN_B) && !btnB_pressed) { btnB_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_B)) btnB_pressed = false;

  if (flappyOver) {
    if (BTN(BTN_A) && !btnA_pressed) { btnA_pressed = true; resetFlappyBird(); }
    if (!BTN(BTN_A)) btnA_pressed = false;
    return;
  }

  // Vogel & Pipes löschen
  tft.fillRect(28, (int)birdY - 1, 12, 12, ST77XX_CYAN);
  tft.fillRect(pipeX, 0, pipeWidth, pipeGapY, ST77XX_CYAN);
  tft.fillRect(pipeX, pipeGapY + pipeGapSize, pipeWidth, 128 - (pipeGapY + pipeGapSize), ST77XX_CYAN);

  birdVelocity += gravity;
  birdY += birdVelocity;

  if (BTN(BTN_A) && !btnA_pressed) { birdVelocity = jumpForce; btnA_pressed = true; }
  if (!BTN(BTN_A)) btnA_pressed = false;

  pipeX -= 3;
  if (pipeX < -pipeWidth) {
    pipeX = 160;
    pipeGapY = random(18, 72);
    flappyScore++;
    // Score aktualisieren
    tft.fillRect(0, 0, 55, 10, ST77XX_CYAN);
    tft.setTextColor(ST77XX_BLACK); tft.setTextSize(1);
    tft.setCursor(2, 2); tft.print("Pts:"); tft.print(flappyScore);
  }

  // Kollision
  bool hitPipe = (pipeX < 40 && pipeX + pipeWidth > 28 &&
                  (birdY < pipeGapY || birdY + 10 > pipeGapY + pipeGapSize));
  if (birdY > 120 || birdY < 0 || hitPipe) {
    flappyOver = true;
    if (flappyScore > highscoreFlappy) {
      highscoreFlappy = flappyScore;
      preferences.putUInt("hs_flappy", highscoreFlappy);
    }
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2);
    tft.setCursor(15, 30); tft.print("GAME OVER");
    tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(20, 58); tft.print("Score:  "); tft.print(flappyScore);
    tft.setCursor(20, 72); tft.print("Best:   "); tft.print(highscoreFlappy);
    tft.setCursor(20, 90); tft.print("A=Retry  B=Menu");
    return;
  }

  // Zeichnen
  tft.fillRect(pipeX, 0, pipeWidth, pipeGapY, ST77XX_GREEN);
  tft.fillRect(pipeX - 2, pipeGapY - 4, pipeWidth + 4, 4, 0x07E0); // Pipe-Kappe
  tft.fillRect(pipeX, pipeGapY + pipeGapSize, pipeWidth, 128 - (pipeGapY + pipeGapSize), ST77XX_GREEN);
  tft.fillRect(pipeX - 2, pipeGapY + pipeGapSize, pipeWidth + 4, 4, 0x07E0);
  tft.fillRect(28, (int)birdY, 10, 10, ST77XX_YELLOW);
  tft.fillRect(36, (int)birdY + 3, 4, 3, ST77XX_RED); // Schnabel

  delay(28);
}

// ==========================================
// PONG
// ==========================================
int paddle1Y, paddle2Y;
float ballX, ballY, ballDX, ballDY;
int score1, score2, pongMode = 0;

void drawPongMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW); tft.setTextSize(2);
  tft.setCursor(15, 15); tft.print("PONG MODE");
  tft.setTextSize(1);
  tft.setCursor(18, 55); tft.setTextColor(pongMode == 0 ? ST77XX_GREEN : ST77XX_WHITE);
  tft.print(pongMode == 0 ? "> 1 Player (vs PC)" : "  1 Player (vs PC)");
  tft.setCursor(18, 75); tft.setTextColor(pongMode == 1 ? ST77XX_GREEN : ST77XX_WHITE);
  tft.print(pongMode == 1 ? "> 2 Player (1v1)" : "  2 Player (1v1)");
  tft.setTextColor(ST77XX_CYAN); tft.setTextSize(1);
  tft.setCursor(18, 105); tft.print("A=Start  B=Menu");
}

void runPongMenu() {
  if ((BTN(BTN_DOWN) || BTN(BTN_UP)) && !btnDown_pressed) {
    pongMode = 1 - pongMode; drawPongMenu(); btnDown_pressed = true; delay(150);
  }
  if (!BTN(BTN_DOWN) && !BTN(BTN_UP)) btnDown_pressed = false;
  if (BTN(BTN_A) && !btnA_pressed) {
    paddle1Y = 44; paddle2Y = 44;
    ballX = 80; ballY = 64;
    ballDX = (random(2) ? 2.5 : -2.5); ballDY = (random(2) ? 2.0 : -2.0);
    score1 = 0; score2 = 0;
    tft.fillScreen(ST77XX_BLACK);
    // Mittellinie
    for (int y = 0; y < 128; y += 8) tft.fillRect(79, y, 2, 4, 0x4208);
    gameState = 3; btnA_pressed = true;
  }
  if (!BTN(BTN_A)) btnA_pressed = false;
  if (BTN(BTN_B)) { gameState = 0; drawMenu(); delay(200); }
}

void runPong() {
  if (BTN(BTN_LEFT) && !btnLeft_pressed) { btnLeft_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_LEFT)) btnLeft_pressed = false;

  tft.fillRect(4, paddle1Y, 4, 22, ST77XX_BLACK);
  tft.fillRect(152, paddle2Y, 4, 22, ST77XX_BLACK);
  tft.fillRect((int)ballX, (int)ballY, 5, 5, ST77XX_BLACK);

  if (BTN(BTN_UP)   && paddle1Y > 0)   paddle1Y -= 3;
  if (BTN(BTN_DOWN) && paddle1Y < 106) paddle1Y += 3;

  if (pongMode == 0) {
    // KI mit leichtem Fehler
    int target = (int)ballY - 8;
    if (abs(target - paddle2Y) > 2) {
      paddle2Y += (target > paddle2Y) ? 2 : -2;
    }
    paddle2Y = constrain(paddle2Y, 0, 106);
  } else {
    if (BTN(BTN_A) && paddle2Y > 0)   paddle2Y -= 3;
    if (BTN(BTN_B) && paddle2Y < 106) paddle2Y += 3;
  }

  ballX += ballDX; ballY += ballDY;

  if (ballY <= 1)   { ballY = 1;   ballDY = abs(ballDY); }
  if (ballY >= 122) { ballY = 122; ballDY = -abs(ballDY); }

  // Schläger-Kollision Paddle 1
  if (ballX <= 8 && ballX >= 4 && ballY + 5 >= paddle1Y && ballY <= paddle1Y + 22) {
    ballDX = abs(ballDX);
    float hitPos = (ballY + 2.5 - (paddle1Y + 11)) / 11.0;
    ballDY = hitPos * 4.0;
  }
  // Schläger-Kollision Paddle 2
  if (ballX + 5 >= 152 && ballX <= 156 && ballY + 5 >= paddle2Y && ballY <= paddle2Y + 22) {
    ballDX = -abs(ballDX);
    float hitPos = (ballY + 2.5 - (paddle2Y + 11)) / 11.0;
    ballDY = hitPos * 4.0;
  }

  if (ballX < 0)   { score2++; ballX = 80; ballY = 64; ballDX = 2.5;  ballDY = 2.0;  delay(400); }
  if (ballX > 160) { score1++; ballX = 80; ballY = 64; ballDX = -2.5; ballDY = -2.0; delay(400); }

  tft.fillRect(4, paddle1Y, 4, 22, ST77XX_WHITE);
  tft.fillRect(152, paddle2Y, 4, 22, ST77XX_WHITE);
  tft.fillRect((int)ballX, (int)ballY, 5, 5, ST77XX_RED);

  // Score
  tft.fillRect(55, 0, 50, 10, ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
  tft.setCursor(58, 1); tft.print(score1); tft.print(" : "); tft.print(score2);

  delay(18);
}

// ==========================================
// SNAKE
// ==========================================
int snakeX[100], snakeY[100];
int snakeLength, snakeDir, scoreSnake;
int foodX, foodY;
bool snakeGameOver;
long lastSnakeTime;

void resetSnake() {
  snakeLength = 3; snakeDir = 1; scoreSnake = 0; snakeGameOver = false;
  for (int i = 0; i < snakeLength; i++) { snakeX[i] = 80 - i*4; snakeY[i] = 64; }
  foodX = random(1, 39) * 4; foodY = random(1, 31) * 4;
  lastSnakeTime = 0;
  tft.fillScreen(ST77XX_BLACK);
  // Rahmen
  tft.drawRect(0, 0, 160, 120, 0x4208);
  tft.fillRect(foodX, foodY, 4, 4, ST77XX_RED);
  tft.setTextColor(0x4208); tft.setTextSize(1);
  tft.setCursor(2, 122); tft.print("Score:0  HS:"); tft.print(highscoreSnake);
}

void runSnake() {
  if (BTN(BTN_B) && !btnB_pressed) { btnB_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_B)) btnB_pressed = false;

  if (snakeGameOver) {
    if (BTN(BTN_A) && !btnA_pressed) { btnA_pressed = true; resetSnake(); }
    if (!BTN(BTN_A)) btnA_pressed = false;
    return;
  }

  // Richtung lesen (keine 180°-Kehrtwendung)
  if (BTN(BTN_UP)    && snakeDir != 2) snakeDir = 0;
  if (BTN(BTN_RIGHT) && snakeDir != 3) snakeDir = 1;
  if (BTN(BTN_DOWN)  && snakeDir != 0) snakeDir = 2;
  if (BTN(BTN_LEFT)  && snakeDir != 1) snakeDir = 3;

  long speed = max(60L, 100L - (long)scoreSnake * 3); // wird schneller
  if (millis() - lastSnakeTime > speed) {
    lastSnakeTime = millis();

    // Schwanz löschen
    tft.fillRect(snakeX[snakeLength-1], snakeY[snakeLength-1], 4, 4, ST77XX_BLACK);

    for (int i = snakeLength - 1; i > 0; i--) { snakeX[i] = snakeX[i-1]; snakeY[i] = snakeY[i-1]; }

    if (snakeDir == 0) snakeY[0] -= 4;
    if (snakeDir == 1) snakeX[0] += 4;
    if (snakeDir == 2) snakeY[0] += 4;
    if (snakeDir == 3) snakeX[0] -= 4;

    // Wand-Kollision
    if (snakeX[0] < 1 || snakeX[0] >= 156 || snakeY[0] < 1 || snakeY[0] >= 116) snakeGameOver = true;
    // Selbst-Kollision
    for (int i = 1; i < snakeLength; i++) {
      if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) snakeGameOver = true;
    }

    if (snakeGameOver) {
      if (scoreSnake > highscoreSnake) { highscoreSnake = scoreSnake; preferences.putUInt("hs_snake", highscoreSnake); }
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_RED); tft.setTextSize(2);
      tft.setCursor(15, 25); tft.print("GAME OVER");
      tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, 55); tft.print("Score:  "); tft.print(scoreSnake);
      tft.setCursor(20, 68); tft.print("Best:   "); tft.print(highscoreSnake);
      tft.setCursor(20, 85); tft.print("A=Retry  B=Menu");
      return;
    }

    // Essen
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
      snakeLength++;
      scoreSnake++;
      // neues Essen (nicht auf Schlange)
      bool onSnake = true;
      while (onSnake) {
        foodX = random(1, 39) * 4; foodY = random(1, 29) * 4;
        onSnake = false;
        for (int i = 0; i < snakeLength; i++) {
          if (snakeX[i] == foodX && snakeY[i] == foodY) { onSnake = true; break; }
        }
      }
      tft.fillRect(0, 120, 160, 8, ST77XX_BLACK);
      tft.setTextColor(0x4208); tft.setTextSize(1);
      tft.setCursor(2, 122); tft.print("Score:"); tft.print(scoreSnake);
      tft.print("  HS:"); tft.print(highscoreSnake);
    }

    tft.fillRect(foodX, foodY, 4, 4, ST77XX_RED);

    // Schlange zeichnen (Kopf heller)
    tft.fillRect(snakeX[0], snakeY[0], 4, 4, ST77XX_WHITE);
    for (int i = 1; i < snakeLength; i++) tft.fillRect(snakeX[i], snakeY[i], 4, 4, ST77XX_GREEN);
  }
}

// ==========================================
// SPACE INVADERS (verbessert: 3 Reihen, Gegner schießen)
// ==========================================
#define INV_ROWS 3
#define INV_COLS 6
int invaderX_player;
int invBulletX, invBulletY;
bool invBulletActive;
int enemiesX[INV_ROWS][INV_COLS];
int enemiesY[INV_ROWS][INV_COLS];
bool enemiesAlive[INV_ROWS][INV_COLS];
int invaderDir;
long lastEnemyMove;
int invEnemySpeed;
// Gegner-Schüsse
int eBulletX[3], eBulletY[3];
bool eBulletActive[3];
long lastEnemyShot;
int scoreInvaders;
bool invadersGameOver;
int invWave;

void resetInvaders() {
  invaderX_player = 75; invBulletActive = false;
  scoreInvaders = 0; invadersGameOver = false; invaderDir = 1;
  invWave = 1; invEnemySpeed = 500;
  for (int r = 0; r < INV_ROWS; r++) {
    for (int c = 0; c < INV_COLS; c++) {
      enemiesX[r][c] = 8 + c * 24;
      enemiesY[r][c] = 8 + r * 14;
      enemiesAlive[r][c] = true;
    }
  }
  for (int i = 0; i < 3; i++) eBulletActive[i] = false;
  lastEnemyMove = 0; lastEnemyShot = 0;
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN); tft.setTextSize(1);
  tft.setCursor(0, 120); tft.print("Score:0");
}

bool invAllDead() {
  for (int r = 0; r < INV_ROWS; r++)
    for (int c = 0; c < INV_COLS; c++)
      if (enemiesAlive[r][c]) return false;
  return true;
}

void runInvaders() {
  if (BTN(BTN_B) && !btnB_pressed) { btnB_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_B)) btnB_pressed = false;

  if (invadersGameOver) {
    if (BTN(BTN_A) && !btnA_pressed) { btnA_pressed = true; resetInvaders(); }
    if (!BTN(BTN_A)) btnA_pressed = false;
    return;
  }

  // Spieler bewegen
  tft.fillRect(invaderX_player, 112, 14, 5, ST77XX_BLACK);
  // Spieler-Schiff
  tft.fillRect(invaderX_player + 5, 110, 4, 3, ST77XX_BLACK);
  if (BTN(BTN_LEFT)  && invaderX_player > 2)   invaderX_player -= 3;
  if (BTN(BTN_RIGHT) && invaderX_player < 144) invaderX_player += 3;

  // Schießen
  if (BTN(BTN_A) && !invBulletActive && !btnA_pressed) {
    invBulletX = invaderX_player + 6; invBulletY = 108;
    invBulletActive = true; btnA_pressed = true;
  }
  if (!BTN(BTN_A)) btnA_pressed = false;

  // Spieler-Schuss bewegen
  if (invBulletActive) {
    tft.fillRect(invBulletX, invBulletY, 2, 5, ST77XX_BLACK);
    invBulletY -= 5;
    if (invBulletY < 0) invBulletActive = false;
    else tft.fillRect(invBulletX, invBulletY, 2, 5, ST77XX_CYAN);
  }

  // Gegner-Schüsse bewegen
  for (int i = 0; i < 3; i++) {
    if (eBulletActive[i]) {
      tft.fillRect(eBulletX[i], eBulletY[i], 2, 4, ST77XX_BLACK);
      eBulletY[i] += 4;
      if (eBulletY[i] > 128) { eBulletActive[i] = false; continue; }
      // Treffer Spieler
      if (eBulletY[i] >= 110 && eBulletX[i] >= invaderX_player && eBulletX[i] <= invaderX_player + 14) {
        invadersGameOver = true;
      }
      tft.fillRect(eBulletX[i], eBulletY[i], 2, 4, ST77XX_YELLOW);
    }
  }

  // Gegner schießen lassen
  if (millis() - lastEnemyShot > 1200) {
    lastEnemyShot = millis();
    // Zufälligen lebenden Gegner auswählen
    int tries = 0;
    while (tries < 10) {
      int r = random(INV_ROWS), c = random(INV_COLS);
      if (enemiesAlive[r][c]) {
        for (int i = 0; i < 3; i++) {
          if (!eBulletActive[i]) {
            eBulletX[i] = enemiesX[r][c] + 4;
            eBulletY[i] = enemiesY[r][c] + 8;
            eBulletActive[i] = true;
            break;
          }
        }
        break;
      }
      tries++;
    }
  }

  // Gegner bewegen
  if (millis() - lastEnemyMove > invEnemySpeed) {
    lastEnemyMove = millis();
    bool hitEdge = false;
    for (int r = 0; r < INV_ROWS; r++) {
      for (int c = 0; c < INV_COLS; c++) {
        if (enemiesAlive[r][c]) {
          tft.fillRect(enemiesX[r][c], enemiesY[r][c], 10, 8, ST77XX_BLACK);
          enemiesX[r][c] += invaderDir * 6;
          if (enemiesX[r][c] >= 148 || enemiesX[r][c] <= 0) hitEdge = true;
        }
      }
    }
    if (hitEdge) {
      invaderDir = -invaderDir;
      for (int r = 0; r < INV_ROWS; r++) {
        for (int c = 0; c < INV_COLS; c++) {
          if (enemiesAlive[r][c]) enemiesY[r][c] += 8;
        }
      }
    }
  }

  // Gegner zeichnen & Kollision prüfen
  for (int r = 0; r < INV_ROWS; r++) {
    for (int c = 0; c < INV_COLS; c++) {
      if (!enemiesAlive[r][c]) continue;

      // Gegner erreicht Boden
      if (enemiesY[r][c] > 105) invadersGameOver = true;

      // Spieler-Schuss trifft Gegner
      if (invBulletActive &&
          invBulletX + 2 >= enemiesX[r][c] && invBulletX <= enemiesX[r][c] + 10 &&
          invBulletY <= enemiesY[r][c] + 8 && invBulletY + 5 >= enemiesY[r][c]) {
        enemiesAlive[r][c] = false;
        invBulletActive = false;
        tft.fillRect(enemiesX[r][c], enemiesY[r][c], 10, 8, ST77XX_BLACK);
        scoreInvaders++;
        // Gegner werden schneller
        invEnemySpeed = max(120, 500 - scoreInvaders * 18);
        tft.fillRect(0, 119, 80, 9, ST77XX_BLACK);
        tft.setTextColor(ST77XX_GREEN); tft.setTextSize(1);
        tft.setCursor(0, 120); tft.print("Score:"); tft.print(scoreInvaders);
        continue;
      }

      // Farbe je nach Reihe
      uint16_t col = (r == 0) ? ST77XX_RED : (r == 1) ? ST77XX_MAGENTA : ST77XX_WHITE;
      // Einfache Alien-Form
      tft.fillRect(enemiesX[r][c]+1, enemiesY[r][c],   8, 2, col);
      tft.fillRect(enemiesX[r][c],   enemiesY[r][c]+2, 10, 4, col);
      tft.fillRect(enemiesX[r][c]+2, enemiesY[r][c]+6, 2, 2, col);
      tft.fillRect(enemiesX[r][c]+6, enemiesY[r][c]+6, 2, 2, col);
    }
  }

  // Nächste Welle
  if (invAllDead()) {
    invWave++;
    invEnemySpeed = max(120, 500 - invWave * 50);
    for (int r = 0; r < INV_ROWS; r++) {
      for (int c = 0; c < INV_COLS; c++) {
        enemiesX[r][c] = 8 + c * 24;
        enemiesY[r][c] = 8 + r * 14;
        enemiesAlive[r][c] = true;
      }
    }
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN); tft.setTextSize(1);
    tft.setCursor(0, 120); tft.print("Score:"); tft.print(scoreInvaders);
  }

  if (invadersGameOver) {
    if (scoreInvaders > highscoreInvaders) {
      highscoreInvaders = scoreInvaders;
      preferences.putUInt("hs_inv", highscoreInvaders);
    }
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2);
    tft.setCursor(15, 25); tft.print("GAME OVER");
    tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(20, 55); tft.print("Score: "); tft.print(scoreInvaders);
    tft.setCursor(20, 68); tft.print("Best:  "); tft.print(highscoreInvaders);
    tft.setCursor(20, 85); tft.print("A=Retry  B=Menu");
    return;
  }

  // Spieler zeichnen
  tft.fillRect(invaderX_player, 112, 14, 5, ST77XX_GREEN);
  tft.fillRect(invaderX_player + 5, 110, 4, 3, ST77XX_GREEN);
  delay(16);
}

// ==========================================
// TETRIS (vollständig: alle 7 Tetrominos, Rotation, Level, Score)
// ==========================================
#define TET_COLS 10
#define TET_ROWS 20
#define TET_SX   29   // Start X auf Display
#define TET_SY   4    // Start Y auf Display
#define TET_BS   6    // Block-Größe in Pixel

// Farben für die 7 Tetrominos
const uint16_t tetColors[7] = {
  ST77XX_CYAN,    // I
  0xFDA0,         // O (gelb)
  ST77XX_MAGENTA, // T
  ST77XX_GREEN,   // S
  ST77XX_RED,     // Z
  0x001F,         // J (blau)
  0xFC00          // L (orange)
};

// Tetromino-Formen: 4 Rotationen × 4 Blöcke × {x,y}
// Format: [Stück][Rotation][Block][xy]
const int8_t PIECES[7][4][4][2] = {
  // I
  {{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}, {{0,2},{1,2},{2,2},{3,2}}, {{1,0},{1,1},{1,2},{1,3}}},
  // O
  {{{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}},
  // T
  {{{1,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{2,1},{1,2}}, {{1,0},{0,1},{1,1},{1,2}}},
  // S
  {{{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}, {{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}},
  // Z
  {{{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}, {{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}},
  // J
  {{{0,0},{0,1},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{1,2}}, {{0,1},{1,1},{2,1},{2,2}}, {{1,0},{1,1},{0,2},{1,2}}},
  // L
  {{{2,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{0,2}}, {{0,0},{1,0},{1,1},{1,2}}}
};

uint16_t tetField[TET_ROWS][TET_COLS]; // 0 = leer, sonst Farbe
int tetPX, tetPY, tetPiece, tetRot;    // aktuelle Position, Stück, Rotation
int tetNextPiece;
long tetLastDrop;
int tetScore, tetLines, tetLevel;
bool tetGameOver;

void tetDrawBlock(int col, int row, uint16_t color) {
  int px = TET_SX + col * TET_BS;
  int py = TET_SY + row * TET_BS;
  if (color == 0) {
    tft.fillRect(px, py, TET_BS, TET_BS, ST77XX_BLACK);
  } else {
    tft.fillRect(px + 1, py + 1, TET_BS - 2, TET_BS - 2, color);
    tft.drawRect(px, py, TET_BS, TET_BS, ST77XX_BLACK);
  }
}

void tetDrawPiece(bool erase) {
  uint16_t col = erase ? 0 : tetColors[tetPiece];
  for (int b = 0; b < 4; b++) {
    int bx = tetPX + PIECES[tetPiece][tetRot][b][0];
    int by = tetPY + PIECES[tetPiece][tetRot][b][1];
    if (by >= 0) tetDrawBlock(bx, by, col);
  }
}

bool tetCollides(int px, int py, int piece, int rot) {
  for (int b = 0; b < 4; b++) {
    int bx = px + PIECES[piece][rot][b][0];
    int by = py + PIECES[piece][rot][b][1];
    if (bx < 0 || bx >= TET_COLS || by >= TET_ROWS) return true;
    if (by >= 0 && tetField[by][bx] != 0) return true;
  }
  return false;
}

void tetLock() {
  for (int b = 0; b < 4; b++) {
    int bx = tetPX + PIECES[tetPiece][tetRot][b][0];
    int by = tetPY + PIECES[tetPiece][tetRot][b][1];
    if (by >= 0) tetField[by][bx] = tetColors[tetPiece];
  }
}

void tetClearLines() {
  int cleared = 0;
  for (int r = TET_ROWS - 1; r >= 0; r--) {
    bool full = true;
    for (int c = 0; c < TET_COLS; c++) {
      if (tetField[r][c] == 0) { full = false; break; }
    }
    if (full) {
      cleared++;
      for (int r2 = r; r2 > 0; r2--)
        for (int c = 0; c < TET_COLS; c++)
          tetField[r2][c] = tetField[r2-1][c];
      for (int c = 0; c < TET_COLS; c++) tetField[0][c] = 0;
      r++; // gleiche Zeile nochmal prüfen
    }
  }
  if (cleared > 0) {
    // Score: 1=100, 2=300, 3=500, 4=800 × Level
    int pts[] = {0, 100, 300, 500, 800};
    tetScore += pts[min(cleared, 4)] * tetLevel;
    tetLines += cleared;
    tetLevel = tetLines / 10 + 1;
    // Spielfeld neu zeichnen
    for (int r = 0; r < TET_ROWS; r++)
      for (int c = 0; c < TET_COLS; c++)
        tetDrawBlock(c, r, tetField[r][c]);
    // Score-Anzeige aktualisieren
    tft.fillRect(0, 0, TET_SX - 1, 128, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
    tft.setCursor(1, 5);  tft.print("SC");
    tft.setCursor(1, 15); tft.print(tetScore);
    tft.setCursor(1, 35); tft.print("LV");
    tft.setCursor(1, 45); tft.print(tetLevel);
    tft.setCursor(1, 65); tft.print("LN");
    tft.setCursor(1, 75); tft.print(tetLines);
  }
}

void tetSpawnNew() {
  tetPiece = tetNextPiece;
  tetNextPiece = random(7);
  tetPX = TET_COLS / 2 - 2;
  tetPY = 0;
  tetRot = 0;

  // Vorschau zeichnen
  tft.fillRect(TET_SX + TET_COLS * TET_BS + 2, 90, 30, 30, ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
  tft.setCursor(TET_SX + TET_COLS * TET_BS + 2, 85); tft.print("NXT");
  for (int b = 0; b < 4; b++) {
    int bx = PIECES[tetNextPiece][0][b][0];
    int by = PIECES[tetNextPiece][0][b][1];
    int px = TET_SX + TET_COLS * TET_BS + 4 + bx * 6;
    int py = 93 + by * 6;
    tft.fillRect(px, py, 5, 5, tetColors[tetNextPiece]);
  }

  if (tetCollides(tetPX, tetPY, tetPiece, tetRot)) {
    tetGameOver = true;
  }
}

void resetTetris() {
  memset(tetField, 0, sizeof(tetField));
  tetScore = 0; tetLines = 0; tetLevel = 1; tetGameOver = false;
  tetNextPiece = random(7);
  tft.fillScreen(ST77XX_BLACK);
  // Rahmen
  tft.drawRect(TET_SX - 1, TET_SY - 1, TET_COLS * TET_BS + 2, TET_ROWS * TET_BS + 2, ST77XX_WHITE);
  // Seitenleiste
  tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
  tft.setCursor(1, 5);  tft.print("SC");
  tft.setCursor(1, 15); tft.print(0);
  tft.setCursor(1, 35); tft.print("LV");
  tft.setCursor(1, 45); tft.print(1);
  tft.setCursor(1, 65); tft.print("LN");
  tft.setCursor(1, 75); tft.print(0);
  tetSpawnNew();
  tetDrawPiece(false);
  tetLastDrop = millis();
  btnLeft_pressed = false; btnRight_pressed = false;
  btnUp_pressed = false; btnDown_pressed = false;
}

void runTetris() {
  if (BTN(BTN_B) && !btnB_pressed) { btnB_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_B)) btnB_pressed = false;

  if (tetGameOver) {
    if (tetScore > highscoreTetris) { highscoreTetris = tetScore; preferences.putUInt("hs_tet", highscoreTetris); }
    if (BTN(BTN_A) && !btnA_pressed) { btnA_pressed = true; resetTetris(); return; }
    if (!BTN(BTN_A)) btnA_pressed = false;
    return;
  }

  bool moved = false;

  // Links
  if (BTN(BTN_LEFT) && !btnLeft_pressed) {
    btnLeft_pressed = true;
    if (!tetCollides(tetPX - 1, tetPY, tetPiece, tetRot)) {
      tetDrawPiece(true);
      tetPX--;
      moved = true;
    }
  }
  if (!BTN(BTN_LEFT)) btnLeft_pressed = false;

  // Rechts
  if (BTN(BTN_RIGHT) && !btnRight_pressed) {
    btnRight_pressed = true;
    if (!tetCollides(tetPX + 1, tetPY, tetPiece, tetRot)) {
      tetDrawPiece(true);
      tetPX++;
      moved = true;
    }
  }
  if (!BTN(BTN_RIGHT)) btnRight_pressed = false;

  // Rotation (BTN_UP)
  if (BTN(BTN_UP) && !btnUp_pressed) {
    btnUp_pressed = true;
    int newRot = (tetRot + 1) % 4;
    // Wall kick: erst normal, dann +1, dann -1
    if (!tetCollides(tetPX, tetPY, tetPiece, newRot)) {
      tetDrawPiece(true); tetRot = newRot; moved = true;
    } else if (!tetCollides(tetPX + 1, tetPY, tetPiece, newRot)) {
      tetDrawPiece(true); tetPX++; tetRot = newRot; moved = true;
    } else if (!tetCollides(tetPX - 1, tetPY, tetPiece, newRot)) {
      tetDrawPiece(true); tetPX--; tetRot = newRot; moved = true;
    }
  }
  if (!BTN(BTN_UP)) btnUp_pressed = false;

  // Soft-Drop
  long dropInterval = max(80L, 600L - (long)(tetLevel - 1) * 50L);
  if (BTN(BTN_DOWN)) dropInterval = 60;

  // Automatischer Fall
  if (millis() - tetLastDrop > dropInterval) {
    tetLastDrop = millis();
    tetDrawPiece(true);
    if (!tetCollides(tetPX, tetPY + 1, tetPiece, tetRot)) {
      tetPY++;
    } else {
      // Einrasten
      tetLock();
      tetClearLines();
      tetSpawnNew();
    }
    moved = true;
  }

  if (moved) tetDrawPiece(false);

  if (tetGameOver) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2);
    tft.setCursor(10, 20); tft.print("GAME OVER");
    tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(15, 55); tft.print("Score: "); tft.print(tetScore);
    tft.setCursor(15, 68); tft.print("Level: "); tft.print(tetLevel);
    tft.setCursor(15, 81); tft.print("Lines: "); tft.print(tetLines);
    tft.setCursor(15, 98); tft.print("Best:  "); tft.print(highscoreTetris);
    tft.setCursor(15, 111); tft.print("A=Retry  B=Menu");
  }
}

// ==========================================
// BREAKOUT (verbessert: Farbreihen, Ballbeschleunigung, Leben)
// ==========================================
#define BO_ROWS  5
#define BO_COLS  8
int boPaddleX;
float boBallX, boBallY, boBallDX, boBallDY;
bool boBricks[BO_ROWS][BO_COLS];
int boScore, boLives;
bool breakoutGameOver, breakoutWon;
const uint16_t boRowColors[BO_ROWS] = {ST77XX_RED, ST77XX_MAGENTA, ST77XX_YELLOW, ST77XX_GREEN, ST77XX_CYAN};

void boDrawBricks() {
  for (int r = 0; r < BO_ROWS; r++) {
    for (int c = 0; c < BO_COLS; c++) {
      int px = c * 20, py = r * 9 + 5;
      if (boBricks[r][c]) tft.fillRect(px + 1, py + 1, 18, 7, boRowColors[r]);
      else                 tft.fillRect(px, py, 20, 9, ST77XX_BLACK);
    }
  }
}

void resetBreakout() {
  boPaddleX = 65; boBallX = 80; boBallY = 90;
  boBallDX = 2.2; boBallDY = -2.5;
  boScore = 0; boLives = 3;
  breakoutGameOver = false; breakoutWon = false;
  for (int r = 0; r < BO_ROWS; r++)
    for (int c = 0; c < BO_COLS; c++)
      boBricks[r][c] = true;
  tft.fillScreen(ST77XX_BLACK);
  boDrawBricks();
  // HUD
  tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
  tft.setCursor(0, 120); tft.print("Pts:0  Lives:3");
}

void runBreakout() {
  if (BTN(BTN_B) && !btnB_pressed) { btnB_pressed = true; gameState = 0; drawMenu(); return; }
  if (!BTN(BTN_B)) btnB_pressed = false;

  if (breakoutGameOver || breakoutWon) {
    if (BTN(BTN_A) && !btnA_pressed) { btnA_pressed = true; resetBreakout(); }
    if (!BTN(BTN_A)) btnA_pressed = false;
    return;
  }

  tft.fillRect(boPaddleX, 112, 30, 4, ST77XX_BLACK);
  tft.fillRect((int)boBallX, (int)boBallY, 4, 4, ST77XX_BLACK);

  if (BTN(BTN_LEFT)  && boPaddleX > 0)   boPaddleX -= 4;
  if (BTN(BTN_RIGHT) && boPaddleX < 130) boPaddleX += 4;

  boBallX += boBallDX; boBallY += boBallDY;

  // Wände
  if (boBallX <= 0)   { boBallX = 0;   boBallDX = abs(boBallDX); }
  if (boBallX >= 155) { boBallX = 155; boBallDX = -abs(boBallDX); }
  if (boBallY <= 0)   { boBallY = 0;   boBallDY = abs(boBallDY); }

  // Schläger-Kollision
  if (boBallY + 4 >= 112 && boBallY <= 116 &&
      boBallX + 4 >= boPaddleX && boBallX <= boPaddleX + 30) {
    boBallDY = -abs(boBallDY);
    boBallY = 111;
    // Winkel je nach Trefferposition
    float hit = (boBallX + 2 - (boPaddleX + 15)) / 15.0;
    boBallDX = hit * 4.5;
    if (abs(boBallDX) < 0.8) boBallDX = (boBallDX >= 0) ? 0.8 : -0.8;
  }

  // Ball verloren
  if (boBallY > 128) {
    boLives--;
    tft.fillRect(0, 119, 160, 9, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
    tft.setCursor(0, 120); tft.print("Pts:"); tft.print(boScore);
    tft.print("  Lives:"); tft.print(boLives);
    if (boLives <= 0) {
      breakoutGameOver = true;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_RED); tft.setTextSize(2);
      tft.setCursor(10, 25); tft.print("GAME OVER");
      tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, 60); tft.print("Score: "); tft.print(boScore);
      tft.setCursor(20, 78); tft.print("A=Retry  B=Menu");
      return;
    }
    boBallX = boPaddleX + 13; boBallY = 90;
    boBallDX = 2.2; boBallDY = -2.5;
    delay(500);
    return;
  }

  // Ziegel-Kollision
  int bCol = (int)(boBallX + 2) / 20;
  int bRow = ((int)(boBallY + 2) - 5) / 9;
  if (bRow >= 0 && bRow < BO_ROWS && bCol >= 0 && bCol < BO_COLS && boBricks[bRow][bCol]) {
    boBricks[bRow][bCol] = false;
    tft.fillRect(bCol * 20, bRow * 9 + 5, 20, 9, ST77XX_BLACK);
    boBallDY = -boBallDY;
    boScore += (BO_ROWS - bRow) * 10; // obere Reihen = mehr Punkte
    // leichte Beschleunigung
    if (abs(boBallDX) < 5.0) boBallDX *= 1.02;
    tft.fillRect(0, 119, 160, 9, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE); tft.setTextSize(1);
    tft.setCursor(0, 120); tft.print("Pts:"); tft.print(boScore);
    tft.print("  Lives:"); tft.print(boLives);

    // Gewonnen?
    bool anyLeft = false;
    for (int r = 0; r < BO_ROWS && !anyLeft; r++)
      for (int c = 0; c < BO_COLS && !anyLeft; c++)
        if (boBricks[r][c]) anyLeft = true;
    if (!anyLeft) {
      breakoutWon = true;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_GREEN); tft.setTextSize(2);
      tft.setCursor(25, 25); tft.print("YOU WIN!");
      tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, 60); tft.print("Score: "); tft.print(boScore);
      tft.setCursor(20, 78); tft.print("A=Play Again");
      return;
    }
  }

  tft.fillRect(boPaddleX, 112, 30, 4, ST77XX_WHITE);
  tft.fillRect((int)boBallX, (int)boBallY, 4, 4, ST77XX_RED);
  delay(14);
}

// ==========================================
// HAUPTMENÜ
// ==========================================
void drawMenu() {
  tft.fillScreen(ST77XX_BLACK);
  // Header
  tft.setTextColor(ST77XX_CYAN); tft.setTextSize(2);
  tft.setCursor(8, 3); tft.print("ESP  BOY");
  tft.drawLine(0, 20, 160, 20, 0x4208);

  tft.setTextSize(1);
  for (int i = 0; i < totalGames; i++) {
    if (menuSelection == i) {
      tft.setTextColor(ST77XX_BLACK);
      tft.fillRect(0, 26 + i * 14, 160, 13, ST77XX_GREEN);
      tft.setCursor(4, 28 + i * 14); tft.print(gameNames[i]);
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.fillRect(0, 26 + i * 14, 160, 13, ST77XX_BLACK);
      tft.setCursor(4, 28 + i * 14); tft.print(gameNames[i]);
    }
  }
  tft.setTextColor(0x4208); tft.setTextSize(1);
  tft.setCursor(2, 120); tft.print("UP/DN=Waehlen  A=Start");
}

void runMenu() {
  if (BTN(BTN_DOWN) && !btnDown_pressed) {
    btnDown_pressed = true;
    menuSelection = (menuSelection + 1) % totalGames;
    drawMenu();
  }
  if (!BTN(BTN_DOWN)) btnDown_pressed = false;

  if (BTN(BTN_UP) && !btnUp_pressed) {
    btnUp_pressed = true;
    menuSelection = (menuSelection - 1 + totalGames) % totalGames;
    drawMenu();
  }
  if (!BTN(BTN_UP)) btnUp_pressed = false;

  if (BTN(BTN_A) && !btnA_pressed) {
    btnA_pressed = true;
    switch (menuSelection) {
      case 0: resetFlappyBird(); gameState = 1; break;
      case 1: drawPongMenu();    gameState = 2; break;
      case 2: resetSnake();      gameState = 4; break;
      case 3: resetInvaders();   gameState = 5; break;
      case 4: resetTetris();     gameState = 6; break;
      case 5: resetBreakout();   gameState = 7; break;
    }
  }
  if (!BTN(BTN_A)) btnA_pressed = false;
  delay(30);
}

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(BTN_A, INPUT_PULLUP); pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP); pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP); pinMode(BTN_RIGHT, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);

  preferences.begin("gameboy", false);
  highscoreFlappy   = preferences.getUInt("hs_flappy", 0);
  highscoreSnake    = preferences.getUInt("hs_snake",  0);
  highscoreTetris   = preferences.getUInt("hs_tet",    0);
  highscoreInvaders = preferences.getUInt("hs_inv",    0);

  drawMenu();
}

void loop() {
  switch (gameState) {
    case 0: runMenu();        break;
    case 1: runFlappyBird();  break;
    case 2: runPongMenu();    break;
    case 3: runPong();        break;
    case 4: runSnake();       break;
    case 5: runInvaders();    break;
    case 6: runTetris();      break;
    case 7: runBreakout();    break;
  }
}
