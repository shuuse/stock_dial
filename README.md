# Stock Price Dial

A physical stock price indicator using an ESP8266 and a servo motor. The dial moves to indicate the current stock price of any configured stock ticker.

## Hardware Requirements
- ESP8266 board
- Servo motor (tested with standard SG90)
- Power supply
- Dial or pointer attachment for servo

## Pin Configuration
- Servo signal: GPIO2
- Servo power: 3.3V
- Servo ground: GND

## Initial Setup
1. Install PlatformIO
2. Clone this repository
3. Copy configuration template:
   ```bash
   cd stock_dial/src
   cp env.template.h env.h
   ```
4. Edit `src/env.h` with your credentials:
   - WiFi SSID and password
   - Alpha Vantage API key (see API Setup below)

## API Setup and Limitations
This project uses Alpha Vantage for stock price data. 

1. Get a free API key at: https://www.alphavantage.co/support/#api-key
2. Free tier limitations:
   - 25 API calls per day maximum
   - 5 API calls per minute

3. To stay safely within these limits, the project:
   - Fetches prices every 80 minutes (18 calls per day)
   - Leaves 7 daily calls as buffer for:
     - Power cycles
     - Network retries
     - System restarts
   - Limits retry attempts to 2 per failed call
   - Caches the last known price if API call fails

Note: If you need more frequent updates, Alpha Vantage offers paid plans:
- Premium: 120 API calls per minute
- Enterprise: 600+ API calls per minute

## Configuration
- Stock settings in `src/config.cpp`:
  - Ticker symbol
  - Price range (min/max for servo movement)
  - Update interval (default: 80 minutes)
- Hardware settings in `src/config.cpp`:
  - Servo pin
  - Pulse width range
  - Movement delays

## Build and Upload
1. Connect your ESP8266
2. Update port in platformio.ini if needed
3. Build and upload using PlatformIO

## Troubleshooting
- If you see "Invalid API call" errors, verify your API key
- If you see "Exceeded API call frequency" errors:
  - The 80-minute interval should prevent this
  - If it occurs, it might indicate clock drift or multiple resets
  - Consider increasing the interval or upgrading your API plan
- The servo will retain its last position if API calls fail
- The system includes 2 retry attempts for failed API calls, with a 10-second delay between attempts

## License
MIT License