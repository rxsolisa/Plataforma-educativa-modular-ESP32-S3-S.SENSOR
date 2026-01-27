#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>
#include <Adafruit_NeoPixel.h>

/* ===== PINES ===== */
#define SDA_PIN 6
#define SCL_PIN 7

#define BUZZER_PIN 8
#define LED_PIN 10

#define BTN_UP    11
#define BTN_OK    12
#define BTN_DOWN  13

/* ===== UMBRALES ===== */
#define CO2_NORMAL   600
#define CO2_ALERTA   800
#define CO2_PELIGRO  900

/* ===== OBJETOS ===== */
Adafruit_SSD1306 display(128, 64, &Wire);
RTC_DS3231 rtc;
Adafruit_AHTX0 aht;
SparkFun_ENS160 ens160;
Adafruit_NeoPixel leds(3, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ===== VARIABLES ===== */
int menuIndex = 0;
int pantalla = 0;

unsigned long alarmaTimer = 0;
bool alarmaEstado = false;

unsigned long btnDownTime = 0;
bool btnDownHeld = false;

/* ===== SONIDOS ===== */
void beep(int f, int t) {
  tone(BUZZER_PIN, f, t);
}

void beepMenu() { beep(2000, 60); }
void beepEnter() { beep(2500, 80); delay(80); beep(3000, 80); }
void beepBack() { beep(800, 150); }

/* ===== SETUP ===== */
void setup() {

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  rtc.begin();
  aht.begin();

  ens160.begin();
  ens160.setOperatingMode(SFE_ENS160_STANDARD);

  leds.begin();
  leds.clear();
  leds.show();

  beepEnter();
}

/* ===== LOOP ===== */
void loop() {
  leerBotones();

  if (pantalla == 0) pantallaMenu();
  if (pantalla == 1) pantallaSensores();
  if (pantalla == 2) pantallaReloj();
  if (pantalla == 3) pantallaInfo();

  delay(100);
}

/* ===== BOTONES ===== */
void leerBotones() {

  if (!digitalRead(BTN_UP) && pantalla == 0) {
    menuIndex--;
    if (menuIndex < 0) menuIndex = 2;
    beepMenu();
    delay(200);
  }

  if (!digitalRead(BTN_DOWN)) {

    if (btnDownTime == 0)
      btnDownTime = millis();

    if (millis() - btnDownTime > 1000 && pantalla != 0) {
      pantalla = 0;
      beepBack();
      btnDownHeld = true;
    }

    if (pantalla == 0 && !btnDownHeld) {
      menuIndex++;
      if (menuIndex > 2) menuIndex = 0;
      beepMenu();
      delay(200);
    }

  } else {
    btnDownTime = 0;
    btnDownHeld = false;
  }

  if (!digitalRead(BTN_OK) && pantalla == 0) {
    pantalla = menuIndex + 1;
    beepEnter();
    delay(300);
  }
}

/* ===== MENU ===== */
void pantallaMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("MENU PRINCIPAL");

  display.setCursor(0, 16);
  display.println(menuIndex == 0 ? "> Sensores" : "  Sensores");
  display.println(menuIndex == 1 ? "> Reloj"    : "  Reloj");
  display.println(menuIndex == 2 ? "> Info"     : "  Info");

  display.display();
}

/* ===== SENSORES ===== */
void pantallaSensores() {

  sensors_event_t hum, temp;
  aht.getEvent(&hum, &temp);

  if (!ens160.checkDataStatus()) return;

  int eco2 = ens160.getECO2();

  /* === LOGICA DE ALARMA === */
  if (eco2 < CO2_NORMAL) {
    leds.fill(leds.Color(0, 150, 0));
    leds.show();
    noTone(BUZZER_PIN);
  }

  else if (eco2 < CO2_ALERTA) {
    leds.fill(leds.Color(150, 150, 0));
    leds.show();
    noTone(BUZZER_PIN);
  }

  else if (eco2 < CO2_PELIGRO) {
    leds.fill(leds.Color(150, 80, 0));
    leds.show();
    beep(1200, 80);
  }

  else {
    if (millis() - alarmaTimer > 300) {
      alarmaTimer = millis();
      alarmaEstado = !alarmaEstado;

      if (alarmaEstado) {
        leds.fill(leds.Color(255, 0, 0));
        tone(BUZZER_PIN, 1800);
      } else {
        leds.clear();
        noTone(BUZZER_PIN);
      }
      leds.show();
    }
  }

  /* === OLED === */
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SENSORES");

  display.print("Temp: ");
  display.print(temp.temperature, 1);
  display.println(" C");

  display.print("Hum: ");
  display.print(hum.relative_humidity, 1);
  display.println(" %");

  display.print("CO2: ");
  display.print(eco2);
  display.println(" ppm");

  display.println("\nDOWN 1s: Volver");
  display.display();

  /* === LOG SERIAL === */
  DateTime now = rtc.now();
  Serial.printf("%02d/%02d/%04d %02d:%02d:%02d | CO2=%d ppm\n",
                now.day(), now.month(), now.year(),
                now.hour(), now.minute(), now.second(),
                eco2);
}

/* ===== RELOJ ===== */
void pantallaReloj() {
  DateTime now = rtc.now();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RELOJ RTC");

  display.printf("%02d/%02d/%04d\n",
                 now.day(), now.month(), now.year());

  display.printf("%02d:%02d:%02d\n",
                 now.hour(), now.minute(), now.second());

  display.println("\nDOWN 1s: Volver");
  display.display();
}

/* ===== INFO ===== */
void pantallaInfo() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("S.SENSOR HMI");
  display.println("ESP32-S3");
  display.println("ENS160 + AHT21");
  display.println("OLED + RTC");
  display.println("\nDOWN 1s: Volver");
  display.display();
}
