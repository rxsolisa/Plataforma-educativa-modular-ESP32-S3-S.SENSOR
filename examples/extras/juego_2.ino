#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define PIN_BOTON 11
#define PIN_BUZZER 8
#define PIN_LEDS 10
Adafruit_NeoPixel leds(3, PIN_LEDS, NEO_GRB + NEO_KHZ800);

// Variables del Dino
int dinoY = 46;
int dinoVelY = 0;
bool saltando = false;
const int gravedad = 2;

// Variables del Obstáculo
int obstaculoX = 128;
int puntaje = 0;
int velocidadJuego = 4;

void setup() {
  Wire.begin(6, 7);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  leds.begin();
  leds.show();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 30);
  display.println("DINO JUMP!");
  display.display();
  delay(2000);
}

void loop() {
  display.clearDisplay();

  // 1. Lógica del Salto
  if (digitalRead(PIN_BOTON) == LOW && !saltando) {
    dinoVelY = -12;
    saltando = true;
    tone(PIN_BUZZER, 800, 50);
  }

  dinoY += dinoVelY;
  dinoVelY += gravedad;

  if (dinoY >= 46) {
    dinoY = 46;
    saltando = false;
  }

  // 2. Movimiento del Obstáculo
  obstaculoX -= velocidadJuego;
  if (obstaculoX < -10) {
    obstaculoX = 128;
    puntaje++;
    if(puntaje % 5 == 0) velocidadJuego++; // Aumenta velocidad cada 5 puntos
  }

  // 3. Dibujar Suelo, Dino y Obstáculo
  display.drawLine(0, 56, 128, 56, SSD1306_WHITE); // Suelo
  display.fillRect(15, dinoY, 10, 10, SSD1306_WHITE); // "Dino"
  display.fillRect(obstaculoX, 46, 6, 10, SSD1306_WHITE); // Cactus

  // Puntaje
  display.setCursor(0, 0);
  display.print("Score: "); display.print(puntaje);

  // 4. Colisión
  if (obstaculoX > 15 && obstaculoX < 25 && dinoY > 36) {
    gameOver();
  }

  display.display();
  delay(30);
}

void gameOver() {
  // Feedback visual y sonoro
  for(int i=0; i<3; i++) leds.setPixelColor(i, leds.Color(255, 0, 0));
  leds.show();
  tone(PIN_BUZZER, 150, 500);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(35, 45);
  display.print("Score: "); display.print(puntaje);
  display.display();
  
  delay(2000);
  // Reiniciar
  puntaje = 0;
  obstaculoX = 128;
  velocidadJuego = 4;
  for(int i=0; i<3; i++) leds.setPixelColor(i, leds.Color(0, 0, 0));
  leds.show();
}
