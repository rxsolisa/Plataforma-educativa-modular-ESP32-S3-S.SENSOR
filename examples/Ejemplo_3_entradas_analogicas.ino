/***************************************************
 * Ejemplo 3 - Entradas analogicas
 * Sensor UV GUVA-S12SD
 *
 * Se lee un valor analogico y se convierte a voltaje
 ***************************************************/

// ================= PIN =================
#define UV_PIN 9   // Pin analogico del sensor UV

// ================= CONSTANTES =================
const float ADC_MAX = 4095.0;
const float VREF = 3.3;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ejemplo 3 - Entrada analogica");
  Serial.println("Sensor UV GUVA-S12SD");
  Serial.println("Ilumina el sensor para ver cambios\n");
}

void loop() {

  // ---------- Lectura ADC ----------
  int adcValue = analogRead(UV_PIN);

  // ---------- Conversion a voltaje ----------
  float voltage = (adcValue / ADC_MAX) * VREF;

  // ---------- Estimacion basica de UV ----------
  // (no es calibracion real, solo educativa)
  float uvIndex = voltage / 0.1;  

  // ---------- Monitor Serial ----------
  Serial.print("ADC: ");
  Serial.print(adcValue);

  Serial.print(" | Voltaje: ");
  Serial.print(voltage, 2);
  Serial.print(" V");

  Serial.print(" | UV estimado: ");
  Serial.println(uvIndex, 1);

  delay(1000);
}
