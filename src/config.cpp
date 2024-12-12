#include "config.h"

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

// Network Configuration
const int UPDATE_INTERVAL = 4800000;  // 80 minutes (4800000 ms) 
                                     // Results in 18 calls per day, leaving buffer for retries
const char* API_ENDPOINT = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%s&apikey=%s";

// Error Handling
const int MAX_RETRY_ATTEMPTS = 2;     // Reduced from 3 to conserve API calls
const int RETRY_DELAY = 10000;        // Delay between retries (10 seconds)