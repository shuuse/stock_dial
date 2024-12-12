#include <Servo.h>

// Servo configuration
const int servoPin = 2;
const int moveDelay = 30;

// Pulse width boundaries
const int maxPriceWidth = 450;   // High stock price ($150)
const int minPriceWidth = 2400;  // Low stock price ($95)

// Stock price range
const float minPrice = 95.0;
const float maxPrice = 150.0;

Servo stockDial;
int currentPulse = minPriceWidth;

// Map stock price to pulse width
int priceToPulse(float price) {
  return map(
    price * 100, 
    minPrice * 100,
    maxPrice * 100,
    minPriceWidth,
    maxPriceWidth
  );
}

void moveToPrice(float price) {
  int targetPulse = priceToPulse(price);
  Serial.printf("Price: $%.2f -> Pulse: %d\n", price, targetPulse);
  
  // Attach, move, wait, then detach
  stockDial.attach(servoPin, maxPriceWidth, minPriceWidth);
  stockDial.writeMicroseconds(targetPulse);
  delay(1500);  // Hold position for 1.5 seconds
  stockDial.detach();
  
  currentPulse = targetPulse;
  Serial.println("Servo detached");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nInitializing NVIDIA stock price dial...");
  
  // Move to starting position
  moveToPrice(minPrice);
  delay(2000);
}

void loop() {
  // Test sequence with 10 price points
  float testPrices[] = {
    95.00,   // Minimum
    101.10,  // 10% range
    107.20,  // 20% range
    113.30,  // 30% range
    119.40,  // 40% range
    122.50,  // Midpoint
    131.00,  // 60% range
    137.10,  // 70% range
    143.20,  // 80% range
    150.00   // Maximum
  };
  
  Serial.println("\nStarting price test sequence...");
  
  // Move through all test prices
  for (float price : testPrices) {
    Serial.printf("\nMoving to $%.2f\n", price);
    moveToPrice(price);
  }
  
  // Move back to minimum
  Serial.println("\nReturning to minimum...");
  moveToPrice(minPrice);
  
  // Wait before next sequence
  delay(5000);
}