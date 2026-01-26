/************************************************************
 *  PROYECTO – MONITOR DE CO2 EN AULA
 *
 *  Funciones:
 *  - Lee eCO2 desde ENS160
 *  - Muestra hora (RTC) y CO2 en OLED
 *  - Cambia color de LEDs según umbrales
 *  - Activa alarma sonora en nivel peligro
 ************************************************************/

#include <Wire.h>
#include <ScioSense_ENS160.h>
#include <RTClib.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// ==================== PINES ====================
#define PIN_LED_WS2812 10
#define NUM_LEDS       3
#define PIN_BUZZER     8

// ==================== UMBRALES CO2 ====================
#define CO2_BAJO_MAX     800
#define CO2_MEDIO_MAX   1200

// ==================== OLED ====================
#define OLED_W 128
#define OLED_H 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

// ==================== OBJETOS ====================
ScioSense_ENS160 ens160;
RTC_DS3231 rtc;
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, OLED_RESET);
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LED_WS2812, NEO_GRB + NEO_KHZ800);

// ==================== SETUP ====================
void setup() {

  Serial.begin(115200);
  delay(1000);

  Wire.begin();

  // --- LEDs ---
  leds.begin();
  leds.clear();
  leds.show();

  // --- OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (1);
  }

  // --- RTC ---
  if (!rtc.begin()) {
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // --- ENS160 ---
  if (!ens160.begin()) {
    while (1);
  }
  ens160.setMode(ENS160_OPMODE_STD);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

// ==================== LOOP ====================
void loop() {

  // -------- Leer RTC --------
  DateTime now = rtc.now();

  // -------- Leer ENS160 --------
  ens160.measure();
  int co2 = ens160.geteCO2();

  // -------- Determinar estado --------
  String estado;
  uint32_t colorLED;
  bool alarma = false;

  if (co2 < CO2_BAJO_MAX) {
    estado = "BAJO";
    colorLED = leds.Color(0, 255, 0);   // Verde
  }
  else if (co2 < CO2_MEDIO_MAX) {
    estado = "MEDIO";
    colorLED = leds.Color(255, 255, 0); // Amarillo
  }
  else {
    estado = "PELIGRO";
    colorLED = leds.Color(255, 0, 0);   // Rojo
    alarma = true;
  }

  // -------- LEDs --------
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, colorLED);
  }
  leds.show();

  // -------- Buzzer --------
  if (alarma) {
    tone(PIN_BUZZER, 2000, 200);
  }

  // -------- OLED --------
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.print(":");
  display.print(now.second());

  display.setCursor(0, 20);
  display.print("CO2: ");
  display.print(co2);
  display.println(" ppm");

  display.setCursor(0, 40);
  display.print("Estado: ");
  display.print(estado);

  display.display();

  // -------- Serial --------
  Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME));
  Serial.print(" | CO2: ");
  Serial.print(co2);
  Serial.print(" | ");
  Serial.println(estado);

  delay(1000);
}
