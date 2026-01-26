
 /***************************************************
 * Ejemplo 6 - RTC DS3231
 * Sincronizacion automatica con la hora del PC
 * SDA = GPIO 6
 * SCL = GPIO 7
 ***************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

// ================= I2C =================
#define SDA_PIN 6
#define SCL_PIN 7

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= RTC =================
RTC_DS3231 rtc;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("Ejemplo 6 - RTC DS3231");
  Serial.println("Sincronizacion con hora del PC");

  // ---------- I2C ----------
  Wire.begin(SDA_PIN, SCL_PIN);

  // ---------- OLED ----------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("❌ Error: OLED no detectada");
    while (1);
  }
  Serial.println("✅ OLED detectada");

  // ---------- RTC ----------
  if (!rtc.begin()) {
    Serial.println("❌ Error: RTC DS3231 no detectado");
    while (1);
  }
  Serial.println("✅ RTC DS3231 detectado");

  // 👉 SOLO ajustar si el RTC perdio energia
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC sin hora");
    Serial.println("⏱ Ajustando con la hora del PC...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("🟢 RTC ya tiene hora");
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("RTC DS3231");
  display.println("Hora sincronizada");
  display.display();

  delay(2000);
}

void loop() {

  DateTime now = rtc.now();

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("Fecha y Hora");
  display.println("------------");

  display.printf("Fecha: %02d/%02d/%04d\n",
                 now.day(), now.month(), now.year());

  display.printf("Hora:  %02d:%02d:%02d\n",
                 now.hour(), now.minute(), now.second());

  display.display();

  // ---------- Serial ----------
  Serial.printf("Fecha/Hora: %02d/%02d/%04d %02d:%02d:%02d\n",
                now.day(), now.month(), now.year(),
                now.hour(), now.minute(), now.second());

  delay(1000);
}
