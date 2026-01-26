/************************************************************
 *  EJEMPLO 02 – LECTURA DE SENSOR I2C
 *  
 *  Sensor:
 *  - ENS160  → Calidad de aire (eCO2, TVOC)
 *  - AHT21   → Temperatura y Humedad
 *
 *  Objetivo:
 *  - Aprender a usar el bus I2C
 *  - Leer sensores ambientales
 *  - Mostrar datos por el Monitor Serial
 *
 *  Pines I2C del kit:
 *  - SDA → Pin 6
 *  - SCL → Pin 7
 ************************************************************/

#include <Wire.h>                 // Librería I2C
#include <ScioSense_ENS160.h>     // Librería ENS160
#include <Adafruit_AHTX0.h>       // Librería AHT21

// ==========================================================
// OBJETOS DE LOS SENSORES
// ==========================================================
ScioSense_ENS160 ens160;   // Sensor de calidad de aire
Adafruit_AHTX0 aht;        // Sensor de temperatura y humedad

// ==========================================================
// FUNCIÓN SETUP
// ==========================================================
void setup() {

  // Iniciar comunicación serial
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Ejemplo 02: Sensor ENS160 + AHT21 ===");

  // --------------------------------------------------------
  // Inicializar bus I2C
  // --------------------------------------------------------
  Wire.begin();   // Usa los pines SDA y SCL definidos en el hardware
  Serial.println("Bus I2C inicializado");

  // --------------------------------------------------------
  // Inicializar sensor AHT21
  // --------------------------------------------------------
  if (!aht.begin()) {
    Serial.println("❌ No se encontro el sensor AHT21");
    while (1);  // Detiene el programa si falla
  }
  Serial.println("✅ Sensor AHT21 encontrado");

  // --------------------------------------------------------
  // Inicializar sensor ENS160
  // --------------------------------------------------------
  if (!ens160.begin()) {
    Serial.println("❌ No se encontro el sensor ENS160");
    while (1);
  }
  Serial.println("✅ Sensor ENS160 encontrado");

  // Configurar el ENS160 en modo estándar
  ens160.setMode(ENS160_OPMODE_STD);
  Serial.println("ENS160 en modo ESTANDAR");

  Serial.println("-------------------------------------");
}

// ==========================================================
// FUNCIÓN LOOP
// ==========================================================
void loop() {

  // --------------------------------------------------------
  // LECTURA DEL AHT21
  // --------------------------------------------------------
  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);

  float tempC = temperatura.temperature;
  float humRH = humedad.relative_humidity;

  // --------------------------------------------------------
  // COMPENSACIÓN DEL ENS160
  // (mejora la precisión usando T° y humedad)
  // --------------------------------------------------------
  ens160.set_envdata(tempC, humRH);

  // --------------------------------------------------------
  // LECTURA DEL ENS160
  // --------------------------------------------------------
  ens160.measure();

  int eco2 = ens160.geteCO2();   // CO2 equivalente (ppm)
  int tvoc = ens160.getTVOC();   // Compuestos orgánicos volátiles (ppb)
  int aqi  = ens160.getAQI();    // Índice de calidad de aire

  // --------------------------------------------------------
  // MOSTRAR DATOS POR MONITOR SERIAL
  // --------------------------------------------------------
  Serial.println("📡 Datos ambientales:");
  Serial.print("Temperatura: ");
  Serial.print(tempC);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humRH);
  Serial.println(" %");

  Serial.print("eCO2: ");
  Serial.print(eco2);
  Serial.println(" ppm");

  Serial.print("TVOC: ");
  Serial.print(tvoc);
  Serial.println(" ppb");

  Serial.print("AQI: ");
  Serial.println(aqi);

  Serial.println("-------------------------------------");

  // Espera antes de la siguiente lectura
  delay(2000);
}
