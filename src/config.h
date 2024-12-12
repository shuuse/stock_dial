#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// Stock Configuration
extern const char* STOCK_TICKER;
extern const float PRICE_MIN;
extern const float PRICE_MAX;

// Servo Configuration
extern const int SERVO_PIN;
extern const int PULSE_WIDTH_MIN;  // For highest price
extern const int PULSE_WIDTH_MAX;  // For lowest price
extern const int MOVE_DELAY;
extern const int HOLD_TIME;
extern const int UPDATE_INTERVAL;

#endif