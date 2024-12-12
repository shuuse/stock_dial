#include "config.h"

// WiFi Configuration
const char* WIFI_SSID = "Kloaknettet";
const char* WIFI_PASSWORD = "34D934D934";

// Stock Configuration
const char* STOCK_TICKER = "NVDA";
const float PRICE_MIN = 95.0;
const float PRICE_MAX = 150.0;

// Servo Configuration
const int SERVO_PIN = 2;
const int PULSE_WIDTH_MIN = 450;   // For highest price
const int PULSE_WIDTH_MAX = 2400;  // For lowest price
const int MOVE_DELAY = 30;         // Delay between movements
const int HOLD_TIME = 1500;        // Time to hold position
const int UPDATE_INTERVAL = 5000;  // Time between updates