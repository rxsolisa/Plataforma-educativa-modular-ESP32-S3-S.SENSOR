#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// --- CONFIGURACIÓN HARDWARE ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_NeoPixel leds(3, 10, NEO_GRB + NEO_KHZ800);

const int PIN_BUZ = 8;
const int BTN_IZQ = 11, BTN_DIS = 12, BTN_DER = 13;

// --- MÚSICA DOOM (E1M1) ---
int melody[] = { 82, 82, 164, 82, 82, 146, 82, 82, 130, 82, 82, 123, 82, 82, 130, 138 };
int baseDurations[] = { 150, 150, 300, 150, 150, 300, 150, 150, 300, 150, 150, 300, 150, 150, 150, 150 };
int noteIdx = 0;
unsigned long nextNoteTime = 0;

// --- VARIABLES DE JUEGO ---
float playerX = 0.0, enemyX = 0.0, enemyZ = 0.0;
int score = 0, health = 3, gunKick = 0;
bool enemyAlive = true, showFlash = false;

void setup() {
  leds.begin();
  leds.setBrightness(30);
  Wire.begin(6, 7); // I2C para ESP32-S3
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  
  pinMode(BTN_IZQ, INPUT_PULLUP); 
  pinMode(BTN_DIS, INPUT_PULLUP); 
  pinMode(BTN_DER, INPUT_PULLUP);
  
  respawnEnemy();
}

void respawnEnemy() {
  enemyZ = 0;
  enemyX = random(-22, 23);
  enemyAlive = true;
}

void playGameOverSound() {
  int deathNotes[] = {100, 80, 60, 40};
  for(int i=0; i<4; i++) {
    tone(PIN_BUZ, deathNotes[i], 300);
    delay(350);
  }
}

void showGameOver() {
  noTone(PIN_BUZ);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 5); display.print("GAME OVER");
  display.setTextSize(1);
  display.setCursor(30, 32); display.print("PUNTOS: "); display.print(score);
  display.setCursor(10, 50); display.print("DISPARA PARA REPETIR");
  display.display(); 
  
  delay(200);
  playGameOverSound();

  bool blink = false;
  while(digitalRead(BTN_DIS) == HIGH) {
    blink = !blink;
    leds.fill(blink ? leds.Color(150, 0, 0) : leds.Color(0, 0, 0));
    leds.show();
    delay(250);
  }
  
  health = 3; score = 0; noteIdx = 0; playerX = 0;
  respawnEnemy();
  display.invertDisplay(false);
}

void drawEnemy(int x, int y, int size) {
  if (!enemyAlive) return;
  display.fillCircle(x, y, size, SSD1306_WHITE);
  display.fillCircle(x, y, size/2, SSD1306_BLACK);
  display.drawLine(x - size, y - size, x - size/2, y - size/2, SSD1306_WHITE);
  display.drawLine(x + size, y - size, x + size/2, y - size/2, SSD1306_WHITE);
}

void drawGun(int kick, bool flash) {
  int xBase = 64 - (playerX / 5);
  int yBase = 64 + kick;
  if (flash) {
    display.fillCircle(xBase, yBase - 22, 8, SSD1306_WHITE);
    display.fillTriangle(xBase-10, yBase-20, xBase, yBase-35, xBase+10, yBase-20, SSD1306_WHITE);
  }
  display.fillRoundRect(xBase - 6, yBase - 18, 5, 20, 2, SSD1306_WHITE);
  display.fillRoundRect(xBase + 1, yBase - 18, 5, 20, 2, SSD1306_WHITE);
  display.fillRect(xBase - 10, yBase - 8, 20, 10, SSD1306_BLACK);
  display.drawRect(xBase - 10, yBase - 8, 20, 10, SSD1306_WHITE);
}

void loop() {
  if (health <= 0) { showGameOver(); return; }
  display.clearDisplay();

  // --- MÚSICA ---
  float speedFactor = 1.0 + (score / 100.0) * 0.1; 
  if (speedFactor > 1.8) speedFactor = 1.8;

  if (millis() > nextNoteTime) {
    noTone(PIN_BUZ);
    int freq = melody[noteIdx];
    int duration = baseDurations[noteIdx] / speedFactor;
    tone(PIN_BUZ, freq, duration * 0.8);
    nextNoteTime = millis() + duration;
    noteIdx = (noteIdx + 1) % 16;
  }

  // --- DIBUJAR MARCADOR (HUD) ---
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print("SCORE: ");
  if(score < 100) display.print("0");
  if(score < 10) display.print("0");
  display.print(score);

  // --- ESCENARIO ---
  int hX = 64 - (playerX / 2);
  display.drawRect(hX - 20, 22, 40, 20, SSD1306_WHITE);
  display.drawLine(0, 10, hX - 20, 22, SSD1306_WHITE); // Ajustado para no pisar el score
  display.drawLine(127, 10, hX + 20, 22, SSD1306_WHITE);
  display.drawLine(0, 63, hX - 20, 42, SSD1306_WHITE);
  display.drawLine(127, 63, hX + 20, 42, SSD1306_WHITE);

  // --- ENEMIGO ---
  if (enemyAlive) {
    enemyZ += (0.35 + (score / 1000.0));
    int size = 1 + (enemyZ / 3);
    int xE = (64 - playerX) + enemyX;
    drawEnemy(xE, 32, size);
    if (enemyZ > 35) {
      health--;
      tone(PIN_BUZ, 40, 200);
      respawnEnemy();
    }
  }

  // --- CONTROLES ---
  if (digitalRead(BTN_IZQ) == LOW) playerX -= 2.2;
  if (digitalRead(BTN_DER) == LOW) playerX += 2.2;
  playerX = constrain(playerX, -45, 45);

  // --- DISPARO Y EFECTO DE MUERTE ---
  if (gunKick > 0) gunKick -= 2;
  if (gunKick < 4) showFlash = false;

  if (digitalRead(BTN_DIS) == LOW && gunKick == 0) {
    tone(PIN_BUZ, 500, 40);
    gunKick = 10;
    showFlash = true;
    int hitPos = (64 - playerX) + enemyX;
    if (enemyAlive && abs(hitPos - 64) < 14) {
      score += 10;
      enemyAlive = false;
      // Sonido explosión
      for(int f = 300; f > 100; f -= 40) { tone(PIN_BUZ, f, 20); delay(10); }
      // Partículas
      display.invertDisplay(true);
      for(int i = 0; i < 15; i++) {
        display.drawPixel((64-playerX)+enemyX+random(-10,10), 32+random(-10,10), SSD1306_WHITE);
      }
      display.display();
      delay(30); 
      display.invertDisplay(false);
      respawnEnemy();
    }
  }

  drawGun(gunKick, showFlash);
  display.drawPixel(64, 32, SSD1306_WHITE);
  
  // LEDs de Vida
  leds.clear();
  for(int i=0; i<health; i++) leds.setPixelColor(i, leds.Color(0, 20, 0));
  leds.show();

  display.display();
  delay(12);
}
