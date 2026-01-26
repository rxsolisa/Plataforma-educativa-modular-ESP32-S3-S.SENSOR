/************************************************************
 *  EJEMPLO 03 – RTC DS3231 + OLED SSD1306
 *
 *  Objetivo:
 *  - Leer fecha y hora desde un RTC
 *  - Mostrar la información en una pantalla OLED
 *  - Comprender el uso de múltiples dispositivos I2C
 *
 *  Bus I2C del kit:
 *  - SDA → Pin 6
 *  - SCL → Pin 7
 ************************************************************/

#include <Wire.h>
#include <RTClib.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==========================================================
// CONFIGURACIÓN DE LA PANTALLA OLED
// ==========================================================
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA  64
#define OLED_RESET     -1      // No se usa pin de reset
#define OLED_DIRECCION 0x3C    // Dirección I2C típica

Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

// ==========================================================
// OBJETO RTC
// ==========================================================
RTC_DS3231 rtc;

// ==========================================================
// FUNCIÓN SETUP
// ==========================================================
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Ejemplo 03: RTC + OLED ===");

  // --------------------------------------------------------
  // Inicializar bus I2C
  // --------------------------------------------------------
  Wire.begin();
  Serial.println("Bus I2C inicializado");

  // --------------------------------------------------------
  // Inicializar pantalla OLED
  // --------------------------------------------------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_DIRECCION)) {
    Serial.println("❌ No se encontro la pantalla OLED");
    while (1);
  }
  Serial.println("✅ Pantalla OLED encontrada");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Mensaje inicial
  display.setCursor(0, 0);
  display.println("Inicializando...");
  display.display();

  // --------------------------------------------------------
  // Inicializar RTC
  // --------------------------------------------------------
  if (!rtc.begin()) {
    Serial.println("❌ No se encontro el RTC DS3231");
    while (1);
  }
  Serial.println("✅ RTC DS3231 encontrado");

  // --------------------------------------------------------
  // Ajustar hora SOLO la primera vez
  // --------------------------------------------------------
  if (rtc.lostPower()) {
    Serial.println("RTC sin energia, ajustando hora...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  delay(1000);
}

// ==========================================================
// FUNCIÓN LOOP
// ==========================================================
void loop() {

  // Leer fecha y hora actual
  DateTime now = rtc.now();

  // --------------------------------------------------------
  // MOSTRAR POR MONITOR SERIAL
  // --------------------------------------------------------
  Serial.print("Fecha: ");
  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print("/");
  Serial.print(now.year());

  Serial.print("  Hora: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // --------------------------------------------------------
  // MOSTRAR EN PANTALLA OLED
  // --------------------------------------------------------
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("RTC DS3231");

  display.setCursor(0, 16);
  display.print("Fecha:");
  display.setCursor(0, 26);
  display.print(now.day());
  display.print("/");
  display.print(now.month());
  display.print("/");
  display.print(now.year());

  display.setCursor(0, 42);
  display.print("Hora:");
  display.setCursor(0, 52);
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.print(":");
  display.print(now.second());

  display.display();

  // Actualizar cada segundo
  delay(1000);
}
