# ESP32 Board Test Project

## Objective

This project provides a simple "getting started" code to test basic functionality across different ESP32 board environments, including ESP32-C3, ESP32-S3, and M5Stack StampS3. The code tests the following features:

1. WiFi connectivity
2. On-board LED control
3. Button functionality

## Project Structure

```
.
├── include/
│   ├── README
│   └── credentials.h
├── lib/
│   └── README
├── src/
│   └── main.cpp
├── .gitignore
├── platformio.ini
└── README.md
```

## Setup Instructions

1. Clone this repository to your local machine.
2. Open the project in PlatformIO IDE (e.g., VS Code with PlatformIO extension).
3. Update the WiFi credentials in `include/credentials.h`:
   ```cpp
      #ifndef CREDENTIALS_H
      #define CREDENTIALS_H

      // WiFi credentials
      const char* ssid = "YourWiFiSSID";
      const char* password = "YourWiFiPassword";

      #endif // CREDENTIALS_H   
   ```
   Replace "YourWiFiSSID" and "YourWiFiPassword" with your actual WiFi network name and password.
4. For the ESP32-S3 board (esp32-s3-matrix), connect an external button to pin 7.

## Usage Instructions

1. Select the appropriate board environment from the PlatformIO toolbar or by changing the `default_envs` in `platformio.ini`:
   - For ESP32-C3, use `esp32-c3u`
   - For ESP32-S3, use `esp32-s3-matrix`
   - For M5Stack StampS3, use `stampS3`

2. Click the "Upload" button in your PlatformIO IDE to compile and upload the code to your board.

3. After successful upload, open the Serial Monitor in your IDE (make sure it's set to 115200 baud).

4. You should see debug messages indicating the WiFi connection attempt and status.

5. Press the on-board button (or external button for ESP32-S3) to toggle the LED on and off. You'll see corresponding messages in the Serial Monitor.

6. The code will periodically check the WiFi status and attempt to reconnect if the connection is lost.

## Board-Specific Information

- ESP32-C3: LED on pin 2, button on pin 9
- ESP32-S3: LED on pin 14, external button should be connected to pin 7
- M5Stack StampS3: LED on pin 21, button on pin 0

## Troubleshooting

- If you encounter compilation errors, make sure you have the latest platform and framework versions installed in PlatformIO.
- If the WiFi connection fails, double-check your credentials in `include/credentials.h`.
- For the ESP32-S3 board, ensure the external button is properly connected to pin 7.

## Contributing

Feel free to fork this project and submit pull requests with improvements or additional features.

## License

This project is open-source and available under the MIT License.