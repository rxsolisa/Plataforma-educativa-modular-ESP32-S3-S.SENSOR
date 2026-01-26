/***************************************************
 * Ejemplo 7 - Proyecto Integrador
 * Monitor de Calidad de Aire en Aula
 *
 * Sensores:
 *  - ENS160 (eCO2)
 *  - AHT21 (Temperatura y Humedad)
 *
 * Interfaz:
 *  - OLED SSD1306
 *  - LEDs WS2812
 *  - Buzzer
 *
 * Tiempo:
 *  - RTC DS3231
 *
 * Registro:
 *  - Salida Serial con fecha y hora
 ***************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// ================= PINES =================
#define SDA_PIN     6
#define SCL_PIN     7
#define LED_PIN     10
#define BUZZER_PIN  8
#define NUM_LEDS    3

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= OBJETOS =================
Adafruit_AHTX0 aht;
SparkFun_ENS160 ens160;
RTC_DS3231 rtc;
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ================= UMBRALES CO2 =================
#define CO2_BAJO   800
#define CO2_MEDIO 1200

void setup() {

  Serial.begin(115200);
  delay(1000);

  // -------- I2C --------
  Wire.begin(SDA_PIN, SCL_PIN);

  // -------- Buzzer --------
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // -------- LEDs --------
  pixels.begin();
  pixels.clear();
  pixels.show();

  // -------- OLED --------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("❌ Error: OLED no detectada");
    while (1);
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // -------- AHT21 --------
  if (!aht.begin()) {
    Serial.println("❌ Error: AHT21 no detectado");
    while (1);
  }

  // -------- ENS160 --------
  if (!ens160.begin()) {
    Serial.println("❌ Error: ENS160 no detectado");
    while (1);
  }
  ens160.setOperatingMode(SFE_ENS160_STANDARD);

  // -------- RTC --------
  if (!rtc.begin()) {
    Serial.println("❌ Error: RTC DS3231 no detectado");
    while (1);
  }

  // Ajustar hora solo si perdió energía
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC sin hora, sincronizando con PC");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // -------- Pantalla inicial --------
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Monitor de Aire");
  display.println("Inicializando...");
  display.display();

  delay(2000);
}

void loop() {

  // ===== LECTURA DE SENSORES =====
  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);

  int eco2 = ens160.getECO2();

  DateTime now = rtc.now();

  // ===== DETERMINAR ESTADO =====
  String estado;
  uint32_t color;
  bool alarma = false;

  if (eco2 < CO2_BAJO) {
    estado = "BAJO";
    color = pixels.Color(0, 150, 0);      // Verde
  }
  else if (eco2 < CO2_MEDIO) {
    estado = "MEDIO";
    color = pixels.Color(150, 150, 0);    // Amarillo
  }
  else {
    estado = "PELIGRO";
    color = pixels.Color(150, 0, 0);      // Rojo
    alarma = true;
  }

  // ===== LEDS =====
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();

  // ===== BUZZER =====
  digitalWrite(BUZZER_PIN, alarma ? HIGH : LOW);

  // ===== OLED =====
  display.clearDisplay();
  display.setCursor(0, 0);

  display.printf("%02d/%02d/%04d %02d:%02d\n",
                 now.day(), now.month(), now.year(),
                 now.hour(), now.minute());

  display.println("----------------");
  display.printf("CO2: %d ppm\n", eco2);
  display.printf("Estado: %s\n", estado.c_str());
  display.printf("Temp: %.1f C\n", temperatura.temperature);
  display.printf("Hum:  %.1f %%\n", humedad.relative_humidity);

  display.display();

  // ===== REGISTRO POR SERIAL (EVENTO) =====
  Serial.printf(
    "%04d-%02d-%02d %02d:%02d:%02d | CO2: %d ppm | Estado: %s\n",
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute(),
    now.second(),
    eco2,
    estado.c_str()
  );

  delay(2000);
}
