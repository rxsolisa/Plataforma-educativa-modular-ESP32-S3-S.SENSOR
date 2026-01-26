/***************************************************
 * Ejemplo 2 - Entradas digitales
 * Uso de botones con pull-up interno
 *
 * Componentes:
 * - 3 botones
 * - LEDs WS2812B como indicador visual
 ***************************************************/

#include <Adafruit_NeoPixel.h>

// ================= PINES =================
#define BTN_UP      11
#define BTN_MID     12
#define BTN_DOWN   13

#define PIN_LED     10
#define NUM_LEDS    3

// ================= OBJETOS =================
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

// ================= VARIABLES =================
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 250; // ms

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ejemplo 2 - Entradas digitales");
  Serial.println("Presiona un boton...");

  // ---------- Configuracion de botones ----------
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_MID, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // ---------- LEDs ----------
  leds.begin();
  leds.clear();
  leds.show();
}

void loop() {

  // --------- Antirrebote basico ----------
  if (millis() - lastPressTime < debounceDelay) return;

  // --------- Boton superior ----------
  if (digitalRead(BTN_UP) == LOW) {
    Serial.println("Boton SUPERIOR presionado");
    setColor(0, 0, 50);   // Azul
    lastPressTime = millis();
  }

  // --------- Boton medio ----------
  if (digitalRead(BTN_MID) == LOW) {
    Serial.println("Boton MEDIO presionado");
    setColor(50, 50, 0);  // Amarillo
    lastPressTime = millis();
  }

  // --------- Boton inferior ----------
  if (digitalRead(BTN_DOWN) == LOW) {
    Serial.println("Boton INFERIOR presionado");
    setColor(0, 50, 0);   // Verde
    lastPressTime = millis();
  }
}

// ================= FUNCION AUXILIAR =================
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}
