#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // Necesaria para forzar canales y potencia de hardware
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// =========================================================
// ✏️ CONFIGURACIÓN DEL ALUMNO 
const char miNombre[10] = "LIDER"; 
// =========================================================

#define PIN_SDA      6
#define PIN_SCL      7
#define PIN_BUZZER   8
#define PIN_LED      10
#define BTN_UP       11
#define BTN_MID      12
#define BTN_DOWN     13

Adafruit_NeoPixel leds(3, PIN_LED, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

uint8_t estadoJuego = 0; // 0: Espera, 1: Jugando, 2: Fin de Ronda
uint8_t botonObjetivo = 0; 
unsigned long tiempoFinRonda = 0;
String mensajeGanador = "";

bool estadoAntUp = HIGH, estadoAntMid = HIGH, estadoAntDown = HIGH;

typedef struct struct_mensaje {
  uint8_t tipoMensaje;   
  char nombreJugador[10]; 
  uint8_t accion;        
} struct_mensaje;

struct_mensaje datosEnvio;
uint8_t direccionBroadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// --- CALLBACK DE RECEPCIÓN ---
void OnDataRecv(const esp_now_recv_info_t * recv_info, const uint8_t *incomingData, int len) {
  struct_mensaje datosRecibidos;
  memcpy(&datosRecibidos, incomingData, sizeof(datosRecibidos));

  if (datosRecibidos.tipoMensaje == 1 && estadoJuego == 0) {
    botonObjetivo = datosRecibidos.accion;
    estadoJuego = 1;
  } 
  else if (datosRecibidos.tipoMensaje == 2 && estadoJuego == 1) {
    mensajeGanador = String(datosRecibidos.nombreJugador) + " GANA!";
    estadoJuego = 2;
    tiempoFinRonda = millis();
  }
}

// --- CALLBACK DE ENVÍO ---
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  // Mantenemos el callback para limpiar de inmediato el buffer TX del S3
}

void setup() {
  Serial.begin(115200);

  Wire.begin(PIN_SDA, PIN_SCL);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_MID, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  
  leds.begin(); leds.clear(); leds.show();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Fallo Pantalla"); for(;;);
  }

  // Configuración de Antena e Infraestructura de Red
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
  WiFi.disconnect();
  
  if (esp_now_init() != ESP_OK) return;
  
  // --- 🔥 MEJORA DE ALCANCE: MÁXIMA POTENCIA DE HARDWARE 🔥 ---
  // Forzamos al amplificador interno a trabajar a 20 dBm (valor 80)
  esp_wifi_set_max_tx_power(80); 
  
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent); 
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, direccionBroadcast, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  strcpy(datosEnvio.nombreJugador, miNombre);
}

void loop() {
  manejarMecanicaJuego();
  actualizarPantallaYLEDs();
}

void manejarMecanicaJuego() {
  bool btnUp = digitalRead(BTN_UP);
  bool btnMid = digitalRead(BTN_MID);
  bool btnDown = digitalRead(BTN_DOWN);

  bool clicUp = (btnUp == LOW && estadoAntUp == HIGH);
  bool clicMid = (btnMid == LOW && estadoAntMid == HIGH);
  bool clicDown = (btnDown == LOW && estadoAntDown == HIGH);
  
  estadoAntUp = btnUp; estadoAntMid = btnMid; estadoAntDown = btnDown;

  if (estadoJuego == 0) {
    if (clicMid) {
      datosEnvio.tipoMensaje = 1;
      datosEnvio.accion = (random(0, 2) == 0) ? BTN_UP : BTN_DOWN;
      
      // --- ⚡ MEJORA: RÁFAGA DE TRANSMISIÓN DE INICIO ⚡ ---
      for (int i = 0; i < 5; i++) {
        esp_now_send(direccionBroadcast, (uint8_t *) &datosEnvio, sizeof(datosEnvio));
        delayMicroseconds(500); // Micropausa para estabilizar la línea de RF
      }
      
      botonObjetivo = datosEnvio.accion;
      estadoJuego = 1;
    }
  } 
  else if (estadoJuego == 1) {
    if ((clicUp && botonObjetivo == BTN_UP) || (clicDown && botonObjetivo == BTN_DOWN)) {
      datosEnvio.tipoMensaje = 2;
      
      // --- ⚡ MEJORA: RÁFAGA DE TRANSMISIÓN DE VICTORIA ⚡ ---
      for (int i = 0; i < 5; i++) {
        esp_now_send(direccionBroadcast, (uint8_t *) &datosEnvio, sizeof(datosEnvio));
        delayMicroseconds(500);
      }
      
      mensajeGanador = "¡TU GANASTE!";
      estadoJuego = 2;
      tiempoFinRonda = millis();
    }
  } 
  else if (estadoJuego == 2) {
    if (millis() - tiempoFinRonda > 4000) {
      estadoJuego = 0;
      noTone(PIN_BUZZER);
    }
  }
}

void actualizarPantallaYLEDs() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  if (estadoJuego == 0) {
    display.setTextSize(1);
    display.setCursor(0, 0); display.print("Jugador: "); display.print(miNombre);
    display.setTextSize(2);
    display.setCursor(5, 25); display.print("Presiona");
    display.setCursor(20, 45); display.print("MEDIO");
    fijarColor(0, 0, 0); 
  } 
  else if (estadoJuego == 1) {
    display.setTextSize(2);
    display.setCursor(15, 25);
    if (botonObjetivo == BTN_UP) {
      display.print("¡ARRIBA!");
      fijarColor(0, 255, 0); 
    } else {
      display.print("¡ABAJO!");
      fijarColor(0, 0, 255); 
    }
  } 
  else if (estadoJuego == 2) {
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.print(mensajeGanador);
    
    if ((millis() / 200) % 2 == 0) {
      fijarColor(255, 0, 255); 
      tone(PIN_BUZZER, 1000);
    } else {
      fijarColor(0, 0, 0);
      noTone(PIN_BUZZER);
    }
  }
  display.display();
}

void fijarColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < 3; i++) leds.setPixelColor(i, leds.Color(r, g, b));
  leds.show();
}
