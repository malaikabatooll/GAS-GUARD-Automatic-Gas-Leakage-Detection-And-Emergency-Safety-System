#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED_PIN   19
#define BUZZ_PIN  18

LiquidCrystal_I2C lcd(0x27, 16, 2);

struct __attribute__((packed)) DataPacket {
  bool gasDetected;
};

volatile DataPacket incomingData;
unsigned long lastActionTime = 0;
bool ledState = false;
bool lastReportedState = false;

void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingDataPtr, int len) {
  if (len == sizeof(incomingData)) {
    memcpy((void*)&incomingData, incomingDataPtr, len);
    Serial.print(" Received: gasDetected = ");
    Serial.println(incomingData.gasDetected ? "YES" : "NO");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Display Node ===");

  // ----- 1. Initialise Wi-Fi in Station mode -----
  WiFi.mode(WIFI_STA);
  Serial.println(" Wi-Fi mode set to STA");

. -----
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, customMac);
  if (err == ESP_OK) {
    Serial.println(" MAC set successfully via esp_wifi_set_mac.");
  } else {
    Serial.printf(" esp_wifi_set_mac failed with error: %d. Using default MAC.\n", err);
  }

  // ----- 3. Read and print the actual MAC (custom or default) -----
  String mac = WiFi.macAddress();
  Serial.print(" MAC Address: ");
  Serial.println(mac);

  if (mac == "00:00:00:00:00:00") {
    Serial.println("  WARNING: MAC is all zeros! Custom MAC not applied.");
    Serial.println("   Please restart the ESP32 and check the default MAC.");
  }

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  Serial.println(" LCD initialised");

  if (esp_now_init() != 0) {
    Serial.println(" ESP-NOW init failed!");
    lcd.clear();
    lcd.print("ESP-NOW Error");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
  Serial.println(" ESP-NOW ready, waiting for data...");
}

void loop() {
  bool currentGas = incomingData.gasDetected;

  if (currentGas != lastReportedState) {
    lastReportedState = currentGas;
    if (currentGas) {
      Serial.println(" GAS LEAK DETECTED!");
    } else {
      Serial.println(" System safe - no gas");
    }
  }

  if (currentGas) {
    lcd.setCursor(0, 0);
    lcd.print("!! ALERT !!     ");
    lcd.setCursor(0, 1);
    lcd.print("GAS LEAK DETECT ");

    if (millis() - lastActionTime >= 200) {
      lastActionTime = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      digitalWrite(BUZZ_PIN, ledState);
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Status: Safe    ");
    lcd.setCursor(0, 1);
    lcd.print("Air Quality OK  ");
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZ_PIN, LOW);
  }
  delay(50);
}
