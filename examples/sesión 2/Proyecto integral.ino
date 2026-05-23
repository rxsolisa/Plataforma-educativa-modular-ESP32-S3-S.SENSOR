#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_AHTX0.h>
#include <DFRobot_ENS160.h>

// --- DEFINICIÓN DE PINES (ESP32-S3) ---
#define PIN_UV       1
#define PIN_SDA      6
#define PIN_SCL      7
#define PIN_BUZZER   8
#define PIN_LED      10
#define BTN_UP       11
#define BTN_MID      12
#define BTN_DOWN     13

// --- CONFIGURACIÓN DE DISPOSITIVOS ---
#define NUM_LEDS 3
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS3231 rtc;
Adafruit_AHTX0 aht;
DFRobot_ENS160_I2C ens160(&Wire, 0x53); // La dirección I2C puede ser 0x52 o 0x53

// --- VARIABLES GLOBALES ---
int menuActual = 1; 
unsigned long tiempoAnterior = 0;
const long intervaloLectura = 500; // Actualiza sensores cada 500ms

// Variables de estado para los botones (Antirrebote simple)
bool estadoAnteriorUp = HIGH;
bool estadoAnteriorMid = HIGH;
bool estadoAnteriorDown = HIGH;

// Variables de almacenamiento de sensores
float temp = 0.0, hum = 0.0, uvVoltaje = 0.0;
uint16_t eco2 = 0, tvoc = 0;
float indiceUV = 0;

void setup() {
  Serial.begin(115200);

  // 1. Inicializar I2C en los pines específicos del ESP32-S3
  Wire.begin(PIN_SDA, PIN_SCL);

  // 2. Configurar Entradas/Salidas
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_MID, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_UV, INPUT);

  // 3. Inicializar Pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Fallo SSD1306");
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

  // 4. Inicializar LEDs
  leds.begin();
  leds.show(); // Apaga todo al inicio

  // 5. Inicializar Sensores (con manejo de errores básico para depuración)
  rtc.begin();
  aht.begin();
  ens160.begin();
  ens160.setPWRMode(ENS160_STANDARD_MODE);
  
  // Si el RTC perdió energía, fijar la hora de compilación
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Pantalla de bienvenida
  display.setCursor(10, 20);
  display.setTextSize(2);
  display.print("EcoWatch");
  display.display();
  delay(2000);
}

void loop() {
  manejarBotones();

  // Uso de millis() para no bloquear la lectura de los botones
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervaloLectura) {
    tiempoAnterior = tiempoActual;
    
    leerSensores();
    actualizarPantalla();
    evaluarAlertas();
  }
}

// --- FUNCIONES SECUNDARIAS ---

void manejarBotones() {
  bool estadoUp = digitalRead(BTN_UP);
  bool estadoMid = digitalRead(BTN_MID);
  bool estadoDown = digitalRead(BTN_DOWN);

  // Detección de flanco de bajada (al presionar)
  if (estadoUp == LOW && estadoAnteriorUp == HIGH) menuActual = 1;
  if (estadoMid == LOW && estadoAnteriorMid == HIGH) menuActual = 2;
  if (estadoDown == LOW && estadoAnteriorDown == HIGH) menuActual = 3;

  estadoAnteriorUp = estadoUp;
  estadoAnteriorMid = estadoMid;
  estadoAnteriorDown = estadoDown;
}

void leerSensores() {
  // Lectura AHT21
  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);
  temp = temperatura.temperature;
  hum = humedad.relative_humidity;

  // Compensación del ENS160 usando datos del AHT21 (Mejora la precisión)
  ens160.setTempAndHum(temp, hum);
  
  // Lectura ENS160
  eco2 = ens160.getECO2();
  tvoc = ens160.getTVOC();

  // Lectura UV Analógica (El ADC del ESP32-S3 es de 12 bits: 0-4095)
  int lecturaRaw = analogRead(PIN_UV);
  uvVoltaje = (lecturaRaw * 3.3) / 4095.0;
  // Conversión aproximada genérica (1V ~ 10 Indice UV)
  indiceUV = uvVoltaje * 10.0; 
}

void actualizarPantalla() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (menuActual == 1) {
    display.println("--- CALIDAD AIRE ---");
    display.print("CO2 (ppm): ");
    display.println(eco2);
    display.print("TVOC (ppb): ");
    display.println(tvoc);
  } 
  else if (menuActual == 2) {
    DateTime now = rtc.now();
    display.println("--- CLIMA & HORA ---");
    display.print("Temp: "); display.print(temp); display.println(" C");
    display.print("Hum:  "); display.print(hum); display.println(" %");
    display.print("Hora: ");
    display.print(now.hour(), DEC); display.print(':');
    display.print(now.minute(), DEC);
  } 
  else if (menuActual == 3) {
    display.println("--- RADIACION UV ---");
    display.print("Voltaje: "); display.print(uvVoltaje); display.println(" V");
    display.print("Indice Aprox: "); display.println(indiceUV);
  }

  display.display();
}

void evaluarAlertas() {
  // Lógica de semáforo y alarmas
  if (eco2 > 1500 || indiceUV > 7.0) {
    // PELIGRO (Rojo)
    fijarColorLED(255, 0, 0);
    tone(PIN_BUZZER, 1000); // Pitido agudo
  } 
  else if (eco2 > 800 || indiceUV > 4.0) {
    // ADVERTENCIA (Amarillo)
    fijarColorLED(255, 255, 0);
    noTone(PIN_BUZZER);
  } 
  else {
    // NORMAL (Verde)
    fijarColorLED(0, 255, 0);
    noTone(PIN_BUZZER);
  }
}

void fijarColorLED(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}
