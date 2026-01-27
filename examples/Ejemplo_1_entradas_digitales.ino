#include <Adafruit_NeoPixel.h>

/* ===== PINES ===== */
#define BUZZER_PIN 8
#define LED_PIN    10
#define NUM_LEDS   3

/* ===== OBJETO LED ===== */
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ===== NOTAS ===== */
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784

/* ===== MELODIA COMPLEJA ===== */
int melody[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5,
  NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4,
  NOTE_F4, NOTE_A4, NOTE_C5, NOTE_D5,
  NOTE_E5, NOTE_G5, NOTE_F5, NOTE_E5
};

int noteDurations[] = {
  200, 200, 200, 300,
  200, 200, 200, 200,
  200, 200, 200, 200,
  200, 200, 200, 300
};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  leds.begin();
  leds.clear();
  leds.show();
}

void loop() {
  for (int i = 0; i < 16; i++) {
    // reproducir la nota
    tone(BUZZER_PIN, melody[i], noteDurations[i]);

    // efecto de LEDs al ritmo
    for (int j = 0; j < NUM_LEDS; j++) {
      // colores dinámicos y brillantes
      int r = random(100, 255);
      int g = random(50, 200);
      int b = random(50, 200);
      leds.setPixelColor(j, leds.Color(r, g, b));
    }
    leds.show();

    delay(noteDurations[i]); // esperar duración de la nota

    // apagar LEDs antes de la siguiente nota
    leds.clear();
    leds.show();
    delay(50);
  }

  delay(500); // descanso entre repeticiones
}
