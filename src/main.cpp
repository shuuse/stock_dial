#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include "config.h"

// System state tracking
struct SystemState {
  float lastPrice = 0;
  unsigned long lastSuccessfulUpdate = 0;
  unsigned long bootTime = 0;
  unsigned long failedRequests = 0;
  unsigned long successfulRequests = 0;
  char lastError[100] = "";
  bool servoAttached = false;
} state;

Servo stockDial;
Ticker watchdogTicker;

// Watchdog reset function
void resetWatchdog() {
  ESP.wdtFeed();
}

// Safe servo operations
void attachServoSafely() {
  if (!state.servoAttached) {
    stockDial.attach(SERVO_PIN, PULSE_WIDTH_MIN, PULSE_WIDTH_MAX);
    state.servoAttached = true;
    delay(100); // Give servo time to power up
  }
}

void detachServoSafely() {
  if (state.servoAttached) {
    stockDial.detach();
    state.servoAttached = false;
    delay(100); // Give servo time to settle
  }
}


// Map stock price to pulse width with boundary checking
int priceToPulse(float price) {
  float constrainedPrice = constrain(price, PRICE_MIN, PRICE_MAX);
  return map(
    constrainedPrice * 100, 
    PRICE_MIN * 100,
    PRICE_MAX * 100,
    PULSE_WIDTH_MAX,
    PULSE_WIDTH_MIN
  );
}

float clampPrice(float price) {
  // Keep a very wide validation range just to catch obviously invalid prices
  if (price < 0 || price > 1000000) {
    return -1;  // Return -1 for obviously invalid prices
  }
  
  // Clamp price to our min/max range
  if (price < PRICE_MIN) {
    Serial.printf("Price $%.2f below range, clamping to $%.2f\n", price, PRICE_MIN);
    return PRICE_MIN;
  }
  if (price > PRICE_MAX) {
    Serial.printf("Price $%.2f above range, clamping to $%.2f\n", price, PRICE_MAX);
    return PRICE_MAX;
  }
  return price;
}

void moveToPrice(float price) {
  float clampedPrice = clampPrice(price);
  if (clampedPrice < 0) {
    snprintf(state.lastError, sizeof(state.lastError), "Invalid price: %.2f", price);
    Serial.println(state.lastError);
    return;
  }

  if (abs(clampedPrice - state.lastPrice) < 0.1) {
    return;
  }

  int targetPulse = priceToPulse(clampedPrice);
  Serial.printf("Original price: $%.2f, Clamped price: $%.2f -> Pulse: %d\n", 
                price, clampedPrice, targetPulse);

  attachServoSafely();
  
  // Smooth movement
  int currentPulse = stockDial.readMicroseconds();
  int steps = abs(targetPulse - currentPulse) / 10;
  
  for (int i = 0; i <= steps; i++) {
    int pulse = map(i, 0, steps, currentPulse, targetPulse);
    stockDial.writeMicroseconds(pulse);
    delay(50);
    resetWatchdog();
  }
  
  stockDial.writeMicroseconds(targetPulse);
  delay(HOLD_TIME);
  detachServoSafely();

  state.lastPrice = price;
}

float getStockPrice() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  char url[200];
  snprintf(url, sizeof(url), API_ENDPOINT, STOCK_TICKER, ALPHAVANTAGE_API_KEY);
  
  Serial.println("Starting HTTPS request...");
  
  http.begin(client, url);
  http.setTimeout(20000);  // 20 second timeout
  http.addHeader("User-Agent", "ESP8266StockDial/1.0");
  http.addHeader("Accept", "application/json");
  
  resetWatchdog();
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      const char* priceStr = doc["Global Quote"]["05. price"];
      if (priceStr) {
        float price = atof(priceStr);
        if (price > 0) {  // Basic sanity check
          state.successfulRequests++;
          state.lastSuccessfulUpdate = millis();
          http.end();
          return price;  // Return raw price, let moveToPrice handle clamping
        } else {
          snprintf(state.lastError, sizeof(state.lastError), "Invalid price: %.2f", price);
        }
      } else {
        snprintf(state.lastError, sizeof(state.lastError), "No price in response");
      }
    } else {
      snprintf(state.lastError, sizeof(state.lastError), "JSON parse error: %s", error.c_str());
    }
  } else {
    snprintf(state.lastError, sizeof(state.lastError), "HTTP error: %d", httpCode);
  }
  
  state.failedRequests++;
  http.end();
  return -1;
}

bool connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  
  Serial.printf("Connecting to WiFi %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
    resetWatchdog();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    return true;
  } else {
    Serial.println("\nWiFi connection failed!");
    return false;
  }
}

void performStartupCalibration() {
  Serial.println("Performing startup calibration...");
  
  attachServoSafely();
  
  // Move to minimum position
  Serial.println("Moving to minimum position...");
  stockDial.writeMicroseconds(PULSE_WIDTH_MAX);  // Remember: MAX pulse = MIN price position
  delay(2000);
  
  // Move to maximum position
  Serial.println("Moving to maximum position...");
  stockDial.writeMicroseconds(PULSE_WIDTH_MIN);  // MIN pulse = MAX price position
  delay(2000);
  
  // Return to center
  Serial.println("Moving to center position...");
  stockDial.writeMicroseconds((PULSE_WIDTH_MAX + PULSE_WIDTH_MIN) / 2);
  delay(1000);
  
  detachServoSafely();
  Serial.println("Calibration complete");
}

void setup() {
  Serial.begin(115200);
  
  // Enable watchdog
  ESP.wdtEnable(WDTO_8S);
  watchdogTicker.attach(1, resetWatchdog);
  
  state.bootTime = millis();
  
  Serial.printf("\nInitializing %s stock price dial...\n", STOCK_TICKER);
  Serial.printf("Compile time: %s %s\n", __DATE__, __TIME__);
  
  // Perform calibration before WiFi connection
  performStartupCalibration();
  
  if (connectToWiFi()) {
    float price = getStockPrice();
    if (price > 0) {
      moveToPrice(price);
    }
  }
}

void loop() {
  static const unsigned long WIFI_CHECK_INTERVAL = 60000;  // Check WiFi every minute
  static unsigned long lastWiFiCheck = 0;
  static unsigned long lastMemoryReport = 0;
  unsigned long currentTime = millis();
  
  // Check WiFi status every minute
  if (currentTime - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    lastWiFiCheck = currentTime;
    
    if (!connectToWiFi()) {
      return;  // Skip this iteration if WiFi connection failed
    }

    // Print memory report every hour
    if (currentTime - lastMemoryReport > 3600000) {
      Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
      Serial.printf("Uptime: %lu minutes\n", currentTime / 60000);
      Serial.printf("Success/Fail ratio: %lu/%lu\n", 
                   state.successfulRequests, 
                   state.failedRequests);
      lastMemoryReport = currentTime;
    }

    // Check if it's time for a price update
    if (currentTime - state.lastSuccessfulUpdate >= UPDATE_INTERVAL) {
      float price = getStockPrice();
      if (price > 0) {
        moveToPrice(price);
      } else {
        Serial.printf("Request failed: %s\n", state.lastError);
      }
    }
  }

  delay(1000);  // Short delay to prevent tight looping
}