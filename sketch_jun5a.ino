#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Preferences.h>

// --- Display & System Pins ---
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

// --- System Variablen ---
int gameState = 0; // 0=Menu, 1=Flappy, 2=PongMenu, 3=Pong, 4=Snake, 5=Invaders, 6=Tetris, 7=Breakout
int menuSelection = 0;
const int totalGames = 6;
String gameNames[6] = {"1. Flappy Bird", "2. Pong", "3. Snake", "4. Space Invaders", "5. Mini-Tetris", "6. Breakout"};

bool btnA_pressed = false, btnB_pressed = false;
bool btnUp_pressed = false, btnDown_pressed = false;

// --- Flappy Bird Variablen ---
float birdY = 60, birdVelocity = 0, gravity = 0.6, jump = -4.5;
int pipeX = 160, pipeGapY = 40, pipeWidth = 20, pipeGapSize = 45;
int score = 0, highscoreFlappy = 0;
bool gameOver = false;

// --- Pong Variablen ---
int paddle1Y = 50, paddle2Y = 50;
float ballX = 80, ballY = 64, ballDX = 3, ballDY = 2;
int score1 = 0, score2 = 0;
int pongMode = 0; 

// --- Snake Variablen ---
int snakeX[50], snakeY[50];
int snakeLength = 3, snakeDir = 1; 
int foodX = 0, foodY = 0, scoreSnake = 0, highscoreSnake = 0;
bool snakeGameOver = false;
long lastSnakeTime = 0;

// --- Space Invaders Variablen ---
int invaderX = 75;
int invBulletX = -1, invBulletY = -1;
bool invBulletActive = false;
int enemiesX[6], enemiesY[6];
bool enemiesAlive[6];
int invaderDir = 1;
long lastEnemyMove = 0;
int scoreInvaders = 0;
bool invadersGameOver = false;

// --- Breakout Variablen ---
int boPaddleX = 60;
float boBallX = 80, boBallY = 100, boBallDX = 2.5, boBallDY = -2.5;
bool boBricks[4][8]; // 4 Reihen, 8 Spalten
int boScore = 0;
bool breakoutGameOver = false;

// --- Mini-Tetris Variablen ---
int tetrisX = 4, tetrisY = 0;
long lastTetrisMove = 0;
bool tetrisField[10][20];
bool tetrisGameOver = false;

// --- Funktions-Prototypen (Verhindern "not declared in this scope" Fehler) ---
void drawMenu();
void runMenu();
void drawPongMenu();
void runPongMenu();
void resetPong();
void runPong();
void resetSnake();
void runSnake();
void resetInvaders();
void runInvaders();
void resetTetris();
void runTetris();
void resetBreakout();
void runBreakout();
void resetFlappyBird();
void runFlappyBird();

void setup() {
  Serial.begin(115200);

  pinMode(BTN_A, INPUT_PULLUP); pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP); pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP); pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(3); 

  preferences.begin("gameboy", false);
  highscoreFlappy = preferences.getUInt("hs_flappy", 0);
  highscoreSnake = preferences.getUInt("hs_snake", 0);

  drawMenu();
}

void loop() {
  if (gameState == 0) runMenu();
  else if (gameState == 1) runFlappyBird();
  else if (gameState == 2) runPongMenu();
  else if (gameState == 3) runPong();
  else if (gameState == 4) runSnake();
  else if (gameState == 5) runInvaders();
  else if (gameState == 6) runTetris();
  else if (gameState == 7) runBreakout();
}

// ==========================================
// 1. HAUPTMENÜ
// ==========================================
void drawMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN); tft.setTextSize(2);
  tft.setCursor(10, 5); tft.print("ESP BOY");
  
  tft.setTextSize(1);
  for(int i = 0; i < totalGames; i++) {
    if(menuSelection == i) {
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(5, 35 + (i * 12)); tft.print("> ");
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(15, 35 + (i * 12));
    }
    tft.print(gameNames[i]);
  }
}

void runMenu() {
  if (digitalRead(BTN_DOWN) == LOW && !btnDown_pressed) {
    menuSelection = (menuSelection + 1) % totalGames; drawMenu(); btnDown_pressed = true;
  } else if (digitalRead(BTN_DOWN) == HIGH) btnDown_pressed = false;

  if (digitalRead(BTN_UP) == LOW && !btnUp_pressed) {
    menuSelection--; if(menuSelection < 0) menuSelection = totalGames - 1; drawMenu(); btnUp_pressed = true;
  } else if (digitalRead(BTN_UP) == HIGH) btnUp_pressed = false;

  if (digitalRead(BTN_A) == LOW && !btnA_pressed) {
    if (menuSelection == 0) { resetFlappyBird(); gameState = 1; }
    else if (menuSelection == 1) { drawPongMenu(); gameState = 2; }
    else if (menuSelection == 2) { resetSnake(); gameState = 4; }
    else if (menuSelection == 3) { resetInvaders(); gameState = 5; }
    else if (menuSelection == 4) { resetTetris(); gameState = 6; }
    else if (menuSelection == 5) { resetBreakout(); gameState = 7; }
    btnA_pressed = true;
  } else if (digitalRead(BTN_A) == HIGH) btnA_pressed = false;
  delay(50);
}

// ==========================================
// 2. PONG MENÜ & SPIEL
// ==========================================
void drawPongMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW); tft.setTextSize(2);
  tft.setCursor(20, 20); tft.print("PONG MODE");
  tft.setTextSize(1);
  tft.setCursor(20, 60); tft.setTextColor(pongMode == 0 ? ST77XX_GREEN : ST77XX_WHITE); tft.print(pongMode == 0 ? "> 1 Player (vs PC)" : "  1 Player (vs PC)");
  tft.setCursor(20, 80); tft.setTextColor(pongMode == 1 ? ST77XX_GREEN : ST77XX_WHITE); tft.print(pongMode == 1 ? "> 2 Player (1v1)" : "  2 Player (1v1)");
}

void runPongMenu() {
  if ((digitalRead(BTN_DOWN) == LOW || digitalRead(BTN_UP) == LOW) && !btnDown_pressed) {
    pongMode = 1 - pongMode; drawPongMenu(); btnDown_pressed = true; delay(150);
  } else if (digitalRead(BTN_DOWN) == HIGH && digitalRead(BTN_UP) == HIGH) btnDown_pressed = false;

  if (digitalRead(BTN_A) == LOW && !btnA_pressed) {
    resetPong(); gameState = 3; btnA_pressed = true;
  } else if (digitalRead(BTN_A) == HIGH) btnA_pressed = false;
  
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); }
}

void resetPong() {
  paddle1Y = 50; paddle2Y = 50; ballX = 80; ballY = 64; ballDX = 3; ballDY = 2; score1 = 0; score2 = 0;
  tft.fillScreen(ST77XX_BLACK);
}

void runPong() {
  if (digitalRead(BTN_LEFT) == LOW) { gameState = 0; drawMenu(); delay(200); return; }

  tft.fillRect(5, paddle1Y, 4, 20, ST77XX_BLACK);
  tft.fillRect(151, paddle2Y, 4, 20, ST77XX_BLACK);
  tft.fillRect((int)ballX, (int)ballY, 4, 4, ST77XX_BLACK);

  if (digitalRead(BTN_UP) == LOW && paddle1Y > 0) paddle1Y -= 3;
  if (digitalRead(BTN_DOWN) == LOW && paddle1Y < 108) paddle1Y += 3;

  if (pongMode == 0) { 
    if (ballY > paddle2Y + 10 && paddle2Y < 108) paddle2Y += 2;
    if (ballY < paddle2Y + 10 && paddle2Y > 0) paddle2Y -= 2;
  } else { 
    if (digitalRead(BTN_A) == LOW && paddle2Y > 0) paddle2Y -= 3;
    if (digitalRead(BTN_B) == LOW && paddle2Y < 108) paddle2Y += 3;
  }

  ballX += ballDX; ballY += ballDY;
  if (ballY <= 0 || ballY >= 124) ballDY = -ballDY;

  if (ballX <= 9 && ballY + 4 >= paddle1Y && ballY <= paddle1Y + 20) { ballX = 9; ballDX = -ballDX; }
  if (ballX >= 147 && ballY + 4 >= paddle2Y && ballY <= paddle2Y + 20) { ballX = 147; ballDX = -ballDX; }

  if (ballX < 0) { score2++; ballX = 80; ballY = 64; delay(500); }
  if (ballX > 160) { score1++; ballX = 80; ballY = 64; delay(500); }

  tft.fillRect(5, paddle1Y, 4, 20, ST77XX_WHITE);
  tft.fillRect(151, paddle2Y, 4, 20, ST77XX_WHITE);
  tft.fillRect((int)ballX, (int)ballY, 4, 4, ST77XX_RED);
  tft.fillRect(60, 0, 40, 10, ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE); tft.setCursor(65, 0); tft.print(score1); tft.print(":"); tft.print(score2);
  delay(20);
}

// ==========================================
// 3. SNAKE
// ==========================================
void resetSnake() {
  snakeLength = 3; snakeDir = 1; scoreSnake = 0; snakeGameOver = false;
  for(int i=0; i<snakeLength; i++) { snakeX[i] = 80 - (i*4); snakeY[i] = 64; }
  foodX = random(1, 39) * 4; foodY = random(1, 31) * 4;
  tft.fillScreen(ST77XX_BLACK);
}

void runSnake() {
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); return; }
  
  if (snakeGameOver) {
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2); tft.setCursor(20, 40); tft.print("GAME OVER");
    return;
  }

  if (digitalRead(BTN_UP) == LOW && snakeDir != 2) snakeDir = 0;
  else if (digitalRead(BTN_RIGHT) == LOW && snakeDir != 3) snakeDir = 1;
  else if (digitalRead(BTN_DOWN) == LOW && snakeDir != 0) snakeDir = 2;
  else if (digitalRead(BTN_LEFT) == LOW && snakeDir != 1) snakeDir = 3;

  if (millis() - lastSnakeTime > 100) { 
    lastSnakeTime = millis();
    tft.fillRect(snakeX[snakeLength-1], snakeY[snakeLength-1], 4, 4, ST77XX_BLACK); 

    for (int i = snakeLength - 1; i > 0; i--) { snakeX[i] = snakeX[i-1]; snakeY[i] = snakeY[i-1]; }

    if (snakeDir == 0) snakeY[0] -= 4; if (snakeDir == 1) snakeX[0] += 4;
    if (snakeDir == 2) snakeY[0] += 4; if (snakeDir == 3) snakeX[0] -= 4;

    if (snakeX[0] < 0 || snakeX[0] >= 160 || snakeY[0] < 0 || snakeY[0] >= 128) snakeGameOver = true;

    for (int i = 1; i < snakeLength; i++) { if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) snakeGameOver = true; }

    if (snakeX[0] == foodX && snakeY[0] == foodY) {
      snakeLength++; scoreSnake++;
      if (scoreSnake > highscoreSnake) { highscoreSnake = scoreSnake; preferences.putUInt("hs_snake", highscoreSnake); }
      foodX = random(1, 39) * 4; foodY = random(1, 31) * 4;
    }

    tft.fillRect(foodX, foodY, 4, 4, ST77XX_RED);
    for (int i = 0; i < snakeLength; i++) tft.fillRect(snakeX[i], snakeY[i], 4, 4, ST77XX_GREEN);
  }
}

// ==========================================
// 4. SPACE INVADERS
// ==========================================
void resetInvaders() {
  invaderX = 75; invBulletActive = false; scoreInvaders = 0; invadersGameOver = false; invaderDir = 1;
  for(int i=0; i<6; i++) { enemiesX[i] = 10 + i*22; enemiesY[i] = 15; enemiesAlive[i] = true; }
  tft.fillScreen(ST77XX_BLACK);
}

void runInvaders() {
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); return; }

  if(invadersGameOver) {
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2); tft.setCursor(20, 40); tft.print("GAME OVER");
    return;
  }

  tft.fillRect(invaderX, 115, 10, 5, ST77XX_BLACK);

  if(digitalRead(BTN_LEFT) == LOW && invaderX > 0) invaderX -= 3;
  if(digitalRead(BTN_RIGHT) == LOW && invaderX < 150) invaderX += 3;

  if(digitalRead(BTN_A) == LOW && !invBulletActive && !btnA_pressed) {
    invBulletX = invaderX + 4; invBulletY = 110; invBulletActive = true; btnA_pressed = true;
  } else if (digitalRead(BTN_A) == HIGH) btnA_pressed = false;

  if(invBulletActive) {
    tft.fillRect(invBulletX, invBulletY, 2, 4, ST77XX_BLACK);
    invBulletY -= 5;
    if(invBulletY < 0) invBulletActive = false;
    else tft.fillRect(invBulletX, invBulletY, 2, 4, ST77XX_YELLOW);
  }

  if(millis() - lastEnemyMove > 400) {
    lastEnemyMove = millis();
    bool hitEdge = false;
    for(int i=0; i<6; i++) {
      if(enemiesAlive[i]) {
        tft.fillRect(enemiesX[i], enemiesY[i], 8, 8, ST77XX_BLACK);
        enemiesX[i] += invaderDir * 5;
        if(enemiesX[i] >= 150 || enemiesX[i] <= 0) hitEdge = true;
      }
    }
    if(hitEdge) {
      invaderDir = -invaderDir;
      for(int i=0; i<6; i++) { if(enemiesAlive[i]) enemiesY[i] += 10; }
    }
  }

  bool allDead = true;
  for(int i=0; i<6; i++) {
    if(enemiesAlive[i]) {
      allDead = false;
      if(invBulletActive && invBulletX >= enemiesX[i] && invBulletX <= enemiesX[i]+8 && invBulletY <= enemiesY[i]+8) {
        enemiesAlive[i] = false; invBulletActive = false;
        tft.fillRect(invBulletX, invBulletY, 2, 4, ST77XX_BLACK);
        tft.fillRect(enemiesX[i], enemiesY[i], 8, 8, ST77XX_BLACK);
        scoreInvaders++;
      } else {
        tft.fillRect(enemiesX[i], enemiesY[i], 8, 8, ST77XX_RED);
        if(enemiesY[i] > 105) invadersGameOver = true;
      }
    }
  }
  
  if(allDead) resetInvaders(); 
  
  tft.fillRect(invaderX, 115, 10, 5, ST77XX_GREEN);
  delay(20);
}

// ==========================================
// 5. MINI-TETRIS
// ==========================================
void resetTetris() {
  memset(tetrisField, 0, sizeof(tetrisField));
  tetrisX = 4; tetrisY = 0; tetrisGameOver = false;
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(54, 14, 52, 102, ST77XX_WHITE); 
}

void runTetris() {
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); return; }

  if(tetrisGameOver) {
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2); tft.setCursor(20, 40); tft.print("GAME OVER");
    return;
  }

  if(digitalRead(BTN_LEFT) == LOW && tetrisX > 0 && !tetrisField[tetrisX-1][tetrisY]) {
     tft.fillRect(55 + tetrisX*5, 15 + tetrisY*5, 5, 5, ST77XX_BLACK);
     tetrisX--; delay(80);
  }
  if(digitalRead(BTN_RIGHT) == LOW && tetrisX < 9 && !tetrisField[tetrisX+1][tetrisY]) {
     tft.fillRect(55 + tetrisX*5, 15 + tetrisY*5, 5, 5, ST77XX_BLACK);
     tetrisX++; delay(80);
  }
  if(digitalRead(BTN_DOWN) == LOW) delay(20); 

  if(millis() - lastTetrisMove > 250) {
    lastTetrisMove = millis();
    tft.fillRect(55 + tetrisX*5, 15 + tetrisY*5, 5, 5, ST77XX_BLACK);

    if(tetrisY < 19 && !tetrisField[tetrisX][tetrisY+1]) {
      tetrisY++;
    } else {
      tetrisField[tetrisX][tetrisY] = true;
      tft.fillRect(55 + tetrisX*5, 15 + tetrisY*5, 5, 5, ST77XX_BLUE); 

      for(int r=19; r>=0; r--) {
        bool fullRow = true;
        for(int c=0; c<10; c++) { if(!tetrisField[c][r]) fullRow = false; }
        
        if(fullRow) {
           for(int r2=r; r2>0; r2--) {
             for(int c=0; c<10; c++) tetrisField[c][r2] = tetrisField[c][r2-1];
           }
           for(int c=0; c<10; c++) tetrisField[c][0] = false;
           
           tft.fillRect(55, 15, 50, 100, ST77XX_BLACK); 
           for(int rr=0; rr<20; rr++) {
             for(int cc=0; cc<10; cc++) {
               if(tetrisField[cc][rr]) tft.fillRect(55+cc*5, 15+rr*5, 5, 5, ST77XX_BLUE);
             }
           }
           r++; 
        }
      }

      tetrisX = 4; tetrisY = 0;
      if(tetrisField[tetrisX][tetrisY]) tetrisGameOver = true;
    }
  }
  tft.fillRect(55 + tetrisX*5, 15 + tetrisY*5, 5, 5, ST77XX_YELLOW); 
}

// ==========================================
// 6. BREAKOUT
// ==========================================
void resetBreakout() {
  boPaddleX = 70; boBallX = 80; boBallY = 80; boBallDX = 2.5; boBallDY = -2.5; boScore = 0; breakoutGameOver = false;
  tft.fillScreen(ST77XX_BLACK);
  
  for(int r=0; r<4; r++) {
    for(int c=0; c<8; c++) {
      boBricks[r][c] = true;
      tft.fillRect(c*20+1, r*10+1, 18, 8, ST77XX_BLUE); 
    }
  }
}

void runBreakout() {
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); return; }

  if(breakoutGameOver) {
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2); tft.setCursor(20, 60); tft.print("GAME OVER");
    return;
  }

  tft.fillRect(boPaddleX, 115, 20, 4, ST77XX_BLACK);
  tft.fillRect((int)boBallX, (int)boBallY, 4, 4, ST77XX_BLACK);

  if(digitalRead(BTN_LEFT) == LOW && boPaddleX > 0) boPaddleX -= 4;
  if(digitalRead(BTN_RIGHT) == LOW && boPaddleX < 140) boPaddleX += 4;

  boBallX += boBallDX; boBallY += boBallDY;

  if(boBallX <= 0 || boBallX >= 156) boBallDX = -boBallDX;
  if(boBallY <= 0) boBallDY = -boBallDY;

  if(boBallY >= 111 && boBallY <= 115 && boBallX+4 >= boPaddleX && boBallX <= boPaddleX+20) {
    boBallDY = -abs(boBallDY); 
    boBallY = 110;
  }

  if(boBallY > 128) breakoutGameOver = true;

  int bCol = (int)boBallX / 20;
  int bRow = (int)boBallY / 10;
  if(bRow >= 0 && bRow < 4 && bCol >= 0 && bCol < 8 && boBricks[bRow][bCol]) {
    boBricks[bRow][bCol] = false;
    tft.fillRect(bCol*20, bRow*10, 20, 10, ST77XX_BLACK); 
    boBallDY = -boBallDY;
    boScore++;
    
    if(boScore >= 32) resetBreakout(); 
  }

  tft.fillRect(boPaddleX, 115, 20, 4, ST77XX_WHITE);
  tft.fillRect((int)boBallX, (int)boBallY, 4, 4, ST77XX_RED);
  delay(20);
}

// ==========================================
// 7. FLAPPY BIRD
// ==========================================
void resetFlappyBird() { birdY=60; birdVelocity=0; pipeX=160; score=0; gameOver=false; tft.fillScreen(ST77XX_CYAN); }

void runFlappyBird() {
  if (digitalRead(BTN_B) == LOW) { gameState = 0; drawMenu(); delay(200); return; }
  
  if (gameOver) {
    tft.setTextColor(ST77XX_RED); tft.setTextSize(2); tft.setCursor(20, 40); tft.print("GAME OVER");
    return;
  }
  
  tft.fillRect(30, (int)birdY, 10, 10, ST77XX_CYAN); tft.fillRect(pipeX, 0, pipeWidth, pipeGapY, ST77XX_CYAN);
  tft.fillRect(pipeX, pipeGapY + pipeGapSize, pipeWidth, 128 - (pipeGapY + pipeGapSize), ST77XX_CYAN);
  
  birdVelocity += gravity; birdY += birdVelocity;
  
  if (digitalRead(BTN_A) == LOW && !btnA_pressed) { birdVelocity = jump; btnA_pressed = true; } 
  else if (digitalRead(BTN_A) == HIGH) btnA_pressed = false;
  
  pipeX -= 3; if (pipeX < -pipeWidth) { pipeX = 160; pipeGapY = random(20, 70); score++; }
  
  if (birdY > 120 || birdY < 0 || (pipeX < 40 && pipeX + pipeWidth > 30 && (birdY < pipeGapY || birdY + 10 > pipeGapY + pipeGapSize))) {
    gameOver = true; if (score > highscoreFlappy) { highscoreFlappy = score; preferences.putUInt("hs_flappy", highscoreFlappy); }
  }
  
  tft.fillRect(30, (int)birdY, 10, 10, ST77XX_YELLOW); tft.fillRect(pipeX, 0, pipeWidth, pipeGapY, ST77XX_GREEN);
  tft.fillRect(pipeX, pipeGapY + pipeGapSize, pipeWidth, 128 - (pipeGapY + pipeGapSize), ST77XX_GREEN);
  tft.setTextColor(ST77XX_BLACK); tft.setTextSize(1); tft.setCursor(5, 5); tft.fillRect(0,0,20,10, ST77XX_CYAN); tft.print(score); delay(30);
}