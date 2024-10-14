# ESP32 RFID Door Control System

This project implements a door control system using an ESP32 microcontroller, an MFRC522 RFID reader, and a web interface. The system allows authorized RFID tags to open the door and provides a web-based interface for remote control and monitoring.

## Features

- RFID-based access control
- Web interface for remote door control
- Real-time door status updates via WebSocket
- Configurable authorized RFID tags
- Support for multiple ESP32 board variants

## Hardware Requirements

- ESP32 board (supported variants: ESP32-C3, ESP32-S3, M5Stack Stamp S3)
- MFRC522 RFID reader
- Relay module for door lock control
- WS2812 LED (optional, for visual feedback)

## Software Dependencies

- Arduino framework
- PlatformIO
- FastLED library
- ArduinoWebsockets library
- MFRC522 library

## Setup Instructions

1. Clone this repository or download the source code.
2. Open the project in PlatformIO.
3. Configure your WiFi credentials in a `credentials.h` file (see `credentials_example.h`).
4. Connect the MFRC522 RFID reader, relay, and LED to your ESP32 board according to the pin definitions in `main.cpp`.
5. Update the `authorized_tags.csv` file in the `data` folder with your authorized RFID tag IDs.
6. Build and upload the project to your ESP32 board.
7. Upload the SPIFFS data (contains the web interface and authorized tags list).

## Usage

1. Power on the ESP32 board.
2. The system will connect to the configured WiFi network and print its IP address to the serial monitor.
3. Access the web interface by navigating to the ESP32's IP address in a web browser.
4. Use authorized RFID tags to open the door by presenting them to the RFID reader.
5. Use the web interface to monitor the door status and open the door remotely.

## Project Structure

- `src/main.cpp`: Main application logic
- `src/WebInterface.h` and `src/WebInterface.cpp`: Web server and WebSocket handling
- `src/RFIDManager.h` and `src/RFIDManager.cpp`: RFID reader management
- `data/index.html`: Web interface HTML file
- `data/authorized_tags.csv`: List of authorized RFID tag IDs

## Customization

- Modify `data/authorized_tags.csv` to add or remove authorized RFID tags.
- Adjust the `DOOR_OPEN_TIME` in `main.cpp` to change how long the door stays open.
- Customize the web interface by editing `data/index.html`.

## Troubleshooting

- If the RFID reader is not detecting tags, check the wiring and ensure the correct SPI pins are used for your ESP32 variant.
- If the web interface is not accessible, verify the WiFi connection and check the ESP32's IP address in the serial monitor.
- For any other issues, check the serial monitor for debug messages and error reports.

## Contributing

Contributions to this project are welcome. Please fork the repository and submit a pull request with your improvements.

## License

This project is released under the MIT License. See the LICENSE file for details.
