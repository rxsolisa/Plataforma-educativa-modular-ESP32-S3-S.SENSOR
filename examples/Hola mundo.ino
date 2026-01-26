#include <Adafruit_NeoPixel.h>

// ===== Definición de pines =====
#define PIN_LED_WS2812 10
#define NUM_LEDS       3

#define PIN_BUZZER     8

#define BTN_SUPERIOR   11
#define BTN_MEDIO      12
#define BTN_INFERIOR   13

// ===== Objeto LED =====
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LED_WS2812, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

  // Inicializar LEDs
  leds.begin();
  leds.clear();
  leds.show();

  // Configurar botones
  pinMode(BTN_SUPERIOR, INPUT_PULLUP);
  pinMode(BTN_MEDIO, INPUT_PULLUP);
  pinMode(BTN_INFERIOR, INPUT_PULLUP);

  // Mensaje inicial
  Serial.println("Hello Kit - Inicializado");

  // LED verde de inicio
  setColor(0, 255, 0);
}

void loop() {

  // Botón superior → LED rojo
  if (digitalRead(BTN_SUPERIOR) == LOW) {
    setColor(255, 0, 0);
    beep(1000, 100);
    Serial.println("Botón superior presionado");
    delay(200);
  }

  // Botón medio → LED azul
  if (digitalRead(BTN_MEDIO) == LOW) {
    setColor(0, 0, 255);
    beep(1500, 100);
    Serial.println("Botón medio presionado");
    delay(200);
  }

  // Botón inferior → LED amarillo
  if (digitalRead(BTN_INFERIOR) == LOW) {
    setColor(255, 255, 0);
    beep(2000, 100);
    Serial.println("Botón inferior presionado");
    delay(200);
  }
}

// ===== Funciones auxiliares =====

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}

void beep(int frecuencia, int duracion) {
  tone(PIN_BUZZER, frecuencia, duracion);
}
