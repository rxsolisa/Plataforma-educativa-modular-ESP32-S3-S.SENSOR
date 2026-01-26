/************************************************************
 *  EJEMPLO 01 – HELLO KIT
 *  
 *  Objetivo:
 *  - Verificar que el kit funciona correctamente
 *  - Aprender a usar:
 *      • Botones (entradas digitales)
 *      • LEDs WS2812B (salidas RGB)
 *      • Buzzer pasivo
 *
 *  Funcionamiento:
 *  - Al iniciar, los LEDs se ponen en color VERDE
 *  - Cada botón cambia el color del LED y emite un sonido
 ************************************************************/

#include <Adafruit_NeoPixel.h>   // Librería para LEDs WS2812B

// ==========================================================
// DEFINICIÓN DE PINES (según el pinout del kit)
// ==========================================================

// LED RGB WS2812B
#define PIN_LED_WS2812 10   // Pin de datos de los LEDs
#define NUM_LEDS       3    // Cantidad de LEDs en el kit

// Buzzer
#define PIN_BUZZER     8    // Buzzer pasivo

// Botones (usan resistencia pull-up interna)
#define BTN_SUPERIOR   11
#define BTN_MEDIO      12
#define BTN_INFERIOR   13

// ==========================================================
// CREACIÓN DEL OBJETO LED
// ==========================================================
// Se indica:
// - Número de LEDs
// - Pin de conexión
// - Tipo de LED (GRB y 800 kHz)
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LED_WS2812, NEO_GRB + NEO_KHZ800);

// ==========================================================
// FUNCIÓN SETUP (se ejecuta una sola vez)
// ==========================================================
void setup() {

  // Inicia la comunicación serial (para mensajes de depuración)
  Serial.begin(115200);
  Serial.println("Iniciando Hello Kit...");

  // Inicializa los LEDs
  leds.begin();
  leds.clear();   // Apaga todos los LEDs
  leds.show();

  // Configuración de botones como entradas con pull-up
  // IMPORTANTE:
  // - Sin presionar → HIGH
  // - Presionado     → LOW
  pinMode(BTN_SUPERIOR, INPUT_PULLUP);
  pinMode(BTN_MEDIO, INPUT_PULLUP);
  pinMode(BTN_INFERIOR, INPUT_PULLUP);

  // Color inicial: VERDE (kit listo)
  setColor(0, 255, 0);

  Serial.println("Kit listo. Presiona un boton.");
}

// ==========================================================
// FUNCIÓN LOOP (se repite constantemente)
// ==========================================================
void loop() {

  // --------------------------------------------------------
  // Botón superior
  // --------------------------------------------------------
  if (digitalRead(BTN_SUPERIOR) == LOW) {  // Botón presionado
    setColor(255, 0, 0);                   // Color ROJO
    beep(1000, 100);                       // Sonido grave
    Serial.println("Boton superior presionado");
    delay(200);                            // Antirrebote simple
  }

  // --------------------------------------------------------
  // Botón medio
  // --------------------------------------------------------
  if (digitalRead(BTN_MEDIO) == LOW) {
    setColor(0, 0, 255);                   // Color AZUL
    beep(1500, 100);                       // Sonido medio
    Serial.println("Boton medio presionado");
    delay(200);
  }

  // --------------------------------------------------------
  // Botón inferior
  // --------------------------------------------------------
  if (digitalRead(BTN_INFERIOR) == LOW) {
    setColor(255, 255, 0);                 // Color AMARILLO
    beep(2000, 100);                       // Sonido agudo
    Serial.println("Boton inferior presionado");
    delay(200);
  }
}

// ==========================================================
// FUNCIONES AUXILIARES
// ==========================================================

/*
 * setColor(r, g, b)
 * Enciende todos los LEDs con el mismo color
 * r, g, b: valores entre 0 y 255
 */
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}

/*
 * beep(frecuencia, duracion)
 * Genera un sonido en el buzzer
 * frecuencia: Hz
 * duracion: milisegundos
 */
void beep(int frecuencia, int duracion) {
  tone(PIN_BUZZER, frecuencia, duracion);
}
