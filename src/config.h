#ifndef CONFIG_H
#define CONFIG_H

#include "env.h"

// Stock Configuration
extern const char* STOCK_TICKER;
extern const float PRICE_MIN;
extern const float PRICE_MAX;

// Servo Configuration
extern const int SERVO_PIN;
extern const int PULSE_WIDTH_MIN;
extern const int PULSE_WIDTH_MAX;
extern const int MOVE_DELAY;
extern const int HOLD_TIME;

// Network Configuration
extern const int UPDATE_INTERVAL;  // Time between price checks in milliseconds
extern const char* API_ENDPOINT;

#endif