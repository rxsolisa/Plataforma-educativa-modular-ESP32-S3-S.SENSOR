/***************************************************
 * Ejemplo 5 - Comunicacion I2C (Visualizacion)
 * Sensores: AHT21 + ENS160
 * Pantalla: OLED SSD1306 128x64
 * SDA = GPIO 6
 * SCL = GPIO 7
 ***************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>

// ================= I2C =================
#define SDA_PIN 6
#define SCL_PIN 7

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= SENSORES =================
Adafruit_AHTX0 aht;
SparkFun_ENS160 ens160;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ejemplo 5 - OLED SSD1306");

  // ---------- I2C ----------
  Wire.begin(SDA_PIN, SCL_PIN);

  // ---------- OLED ----------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("❌ Error: OLED no detectada");
    while (1);
  }
  Serial.println("✅ OLED detectada");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Plataforma ESP32");
  display.println("Inicializando...");
  display.display();

  // ---------- AHT21 ----------
  if (!aht.begin()) {
    Serial.println("❌ Error: AHT21 no detectado");
    while (1);
  }
  Serial.println("✅ AHT21 detectado");

  // ---------- ENS160 ----------
  if (!ens160.begin()) {
    Serial.println("❌ Error: ENS160 no detectado");
    while (1);
  }
  Serial.println("✅ ENS160 detectado");

  ens160.setOperatingMode(SFE_ENS160_STANDARD);

  delay(2000);
}

void loop() {

  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);

  int eco2 = 0, tvoc = 0, aqi = 0;

  if (ens160.checkDataStatus()) {
    eco2 = ens160.getECO2();
    tvoc = ens160.getTVOC();
    aqi  = ens160.getAQI();
  }

  // ---------- OLED ----------
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("Datos Ambientales");
  display.println("----------------");

  display.print("Temp: ");
  display.print(temperatura.temperature, 1);
  display.println(" C");

  display.print("Hum:  ");
  display.print(humedad.relative_humidity, 1);
  display.println(" %");

  display.print("eCO2: ");
  display.print(eco2);
  display.println(" ppm");

  display.print("TVOC: ");
  display.print(tvoc);
  display.println(" ppb");

  display.print("AQI:  ");
  display.println(aqi);

  display.display();

  // ---------- Serial ----------
  Serial.println("OLED actualizada");

  delay(2000);
}
