#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// --- CONFIGURACIÓN DE HARDWARE ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PIN_BUZZER 8
#define PIN_LEDS   10
#define NUM_LEDS   3
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

#define PIN_BOTON  11 // Botón superior

// --- VARIABLES DEL JUEGO ---
int pixelX = 0;
int direccion = 1;
int velocidad = 2;
int score = 0;
int vidas = 3;
bool juegoActivo = true;

// Zona de captura (centro de la pantalla)
const int zonaInicio = 54;
const int zonaFin = 74;

void setup() {
  Serial.begin(115200);
  
  // Inicializar Pantalla
  Wire.begin(6, 7); // SDA: 6, SCL: 7
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;);
  }
  
  // Inicializar LEDs y Buzzer
  leds.begin();
  leds.show(); 
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BOTON, INPUT_PULLUP);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("CATCH THE PIXEL!");
  display.display();
  delay(2000);
}

void loop() {
  if (juegoActivo) {
    jugar();
  } else {
    // Pantalla de Game Over
    display.clearDisplay();
    display.setCursor(35, 15);
    display.setTextSize(2);
    display.println("GAME");
    display.setCursor(35, 35);
    display.println("OVER");
    display.setTextSize(1);
    display.setCursor(30, 55);
    display.print("Score final: ");
    display.println(score);
    display.display();
    
    if (digitalRead(PIN_BOTON) == LOW) {
      // Reiniciar juego
      score = 0;
      vidas = 3;
      velocidad = 2;
      juegoActivo = true;
      delay(500);
    }
  }
}

void jugar() {
  display.clearDisplay();

  // Dibujar zona de captura
  display.drawRect(zonaInicio, 20, 20, 20, SSD1306_WHITE);
  
  // Dibujar información
  display.setCursor(0, 0);
  display.print("Score: "); display.print(score);
  display.setCursor(80, 0);
  display.print("Vidas: "); display.print(vidas);

  // Mover píxel
  pixelX += (direccion * velocidad);
  if (pixelX >= 120 || pixelX <= 0) {
    direccion *= -1;
  }
  display.fillRect(pixelX, 28, 5, 5, SSD1306_WHITE);

  // Leer botón
  if (digitalRead(PIN_BOTON) == LOW) {
    if (pixelX >= zonaInicio && pixelX <= zonaFin) {
      // ¡ACIERTO!
      score++;
      velocidad++;
      tone(PIN_BUZZER, 1000, 100);
      cambiarColorLEDs(0, 255, 0); // Verde
      delay(200);
    } else {
      // ¡FALLO!
      vidas--;
      tone(PIN_BUZZER, 200, 300);
      cambiarColorLEDs(255, 0, 0); // Rojo
      delay(500);
      if (vidas <= 0) juegoActivo = false;
    }
    cambiarColorLEDs(0, 0, 0); // Apagar
  }

  display.display();
  delay(10);
}

void cambiarColorLEDs(int r, int g, int b) {
  for(int i=0; i<NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}
