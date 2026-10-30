#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

#define GAS_PIN    D2
#define LED_PIN    D5
#define BUZZ_PIN   D6
#define SERVO_PIN  D7
#define FAN_PIN    D8

Servo myServo;

uint8_t broadcastAddress[] = {0x02, 0x00, 0x00, 0x12, 0x34, 0x56};

struct __attribute__((packed)) DataPacket {
  bool gasDetected;
};

DataPacket packet;
unsigned long lastActionTime = 0;
bool ledState = false;
bool lastGasState = false;   // for serial change detection

void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Send status: ");
  Serial.println(sendStatus == 0 ? "SUCCESS" : "FAIL");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP8266 Sensor Node ===");

  pinMode(GAS_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);
  Serial.println("Servo at 0° (closed)");

  WiFi.mode(WIFI_STA);
  Serial.print(" ESP8266 MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  Serial.print("Peer added with MAC: ");
  for (int i=0; i<6; i++) {
    Serial.printf("%02X", broadcastAddress[i]);
    if (i<5) Serial.print(":");
  }
  Serial.println();
}

void loop() {
  bool gasDetected = (digitalRead(GAS_PIN) == HIGH);
  packet.gasDetected = gasDetected;


  esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));

  if (gasDetected != lastGasState) {
    lastGasState = gasDetected;
    Serial.print("🟢 Gas state changed: ");
    Serial.println(gasDetected ? "DETECTED" : "CLEAR");
  }

  if (gasDetected) {
    digitalWrite(FAN_PIN, HIGH);
    myServo.write(90);
    Serial.println("    Fan ON, Servo -> 90°");

    if (millis() - lastActionTime >= 200) {
      lastActionTime = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      digitalWrite(BUZZ_PIN, ledState);
    }
  } else {
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZ_PIN, LOW);
    myServo.write(0);
    // Only print actions when they occur (avoid spam)
    static bool wasClear = true;
    if (!wasClear) {
      Serial.println("   All reset (fan OFF, servo 0°)");
      wasClear = true;
    }
  }
  delay(50);
}
