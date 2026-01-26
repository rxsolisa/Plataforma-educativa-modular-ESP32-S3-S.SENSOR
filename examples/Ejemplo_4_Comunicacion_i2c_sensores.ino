/***************************************************
 * Ejemplo 4 - Comunicacion I2C
 * Sensores: ENS160 + AHT21
 * SDA = GPIO 6
 * SCL = GPIO 7
 ***************************************************/

// ================= LIBRERIAS =================
#include <Wire.h>
#include <Adafruit_Sensor.h>     // <- ESTA ES LA CLAVE
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>

// ================= PINES I2C =================
#define SDA_PIN 6
#define SCL_PIN 7

// ================= OBJETOS =================
Adafruit_AHTX0 aht;
SparkFun_ENS160 ens160;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ejemplo 4 - Comunicacion I2C");
  Serial.println("Sensores: ENS160 + AHT21");

  // ---------- I2C ----------
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("I2C iniciado en SDA=6, SCL=7");

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

  Serial.println("Sensores listos.\n");
}

void loop() {

  // ---------- AHT21 ----------
  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);

  // ---------- ENS160 ----------
  if (ens160.checkDataStatus()) {

    int eco2 = ens160.getECO2();  // ppm
    int tvoc = ens160.getTVOC();  // ppb
    int aqi  = ens160.getAQI();   // 1 a 5

    Serial.println("----- Datos ambientales -----");

    Serial.print("Temperatura: ");
    Serial.print(temperatura.temperature);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(humedad.relative_humidity);
    Serial.println(" %");

    Serial.print("eCO2: ");
    Serial.print(eco2);
    Serial.println(" ppm");

    Serial.print("TVOC: ");
    Serial.print(tvoc);
    Serial.println(" ppb");

    Serial.print("AQI: ");
    Serial.println(aqi);

    Serial.println("------------------------------\n");
  }

  delay(2000);
}
