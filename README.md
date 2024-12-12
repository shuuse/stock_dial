# Stock Price Dial

A physical stock price indicator using an ESP8266 and a servo motor. The dial moves to indicate the current stock price of any configured stock ticker.

## Hardware Requirements
- ESP8266 board
- Servo motor
- Power supply

## Pin Configuration
- Servo signal: GPIO2
- Servo power: 3.3V
- Servo ground: GND

## Setup
1. Install PlatformIO
2. Clone this repository
3. Update config.h with your:
   - WiFi credentials
   - Stock ticker
   - Price range
4. Build and upload to your ESP8266

## Configuration
Edit `src/config.h` to set:
- Stock ticker symbol
- Minimum and maximum price range for the dial
- WiFi credentials

## License
MIT License