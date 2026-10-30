# GAS-GUARD-Automatic-Gas-Leakage-Detection-And-Emergency-Safety-System
An ESP32 and ESP8266 based Smart Automatic Gas Leakage Detection &amp;  Safety System using an MQ-5 gas sensor, servo motor, buzzer, and I2C LCD to detect gas leaks, alert users, and automatically shut off the gas valve.
# GAS-GUARD
### Automatic Gas Leakage Detection & Emergency Safety System

## Features

- **MQ-5 Gas Sensor:** Detects combustible gas conditions through its digital output.
- **ESP8266:** Processes the gas sensor input and controls the emergency response.
- **Servo Motor:** Operates the model gas valve according to the programmed response.
- **Brushless DC Exhaust Fan:** Provides ventilation when gas is detected.
- **LED Indicator (ESP8266):** Provides visual indication of the detected gas condition.
- **Buzzer (ESP8266):** Produces an audible warning.
- **ESP-NOW Communication:** Transmits gas detection status wirelessly from ESP8266 to ESP32.
- **ESP32:** Receives gas detection status and manages the LCD and additional alerts.
- **16×2 I²C LCD:** Displays system status and gas leakage warnings.
- **LED Indicator (ESP32):** Provides additional emergency indication.
- **Buzzer (ESP32):** Produces an additional audible warning.

## Working Workflow

GAS-GUARD follows a four-stage emergency response mechanism:

**DETECT → ISOLATE → VENTILATE → WARN**

1. **DETECT:** The MQ-5 sensor detects combustible gas conditions.
2. **ISOLATE:** The servo motor operates the model gas valve.
3. **VENTILATE:** The brushless DC exhaust fan activates.
4. **WARN:** The ESP8266 and ESP32 activate their respective LED and buzzer alerts, while the ESP32 displays the warning on the I²C LCD.

## ESP8266 Pin Mapping

| Device | Board Pin | GPIO |
|---|---|---:|
| MQ-5 Gas Sensor | D2 | 4 |
| LED | D5 | 14 |
| Buzzer | D6 | 12 |
| Servo Motor | D7 | 13 |
| Exhaust Fan Control | D8 | 15 |

## ESP32 Pin Mapping

| Device | GPIO / Connection |
|---|---|
| LED | GPIO 19 |
| Buzzer | GPIO 18 |
| LCD SDA | I²C SDA |
| LCD SCL | I²C SCL |

**LCD Configuration:**
- Display: 16×2
- Communication: I²C
- I²C Address: `0x27`

## Libraries Used

**ESP8266:**
- `ESP8266WiFi.h`
- `espnow.h`
- `Servo.h`

**ESP32:**
- `WiFi.h`
- `esp_now.h`
- `esp_wifi.h`
- `Wire.h`
- `LiquidCrystal_I2C.h`
