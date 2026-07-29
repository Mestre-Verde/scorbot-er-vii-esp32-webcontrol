# ESP32 API for WebSocket Control of the Scorbot-ER VII

## For Schools 🏫

This project was created to give a second life to the classic **Scorbot-ER VII** educational robot. The goal is to modernize the robot while preserving its educational value, making it easier to use in classrooms and laboratories.

---

# ESP32 API 📁

This project is developed using the **ESP-IDF** framework. PlatfomrIO with Arduino workspace was used in the previus versions.

The codebase follows a modular architecture based on ESP-IDF components, making it easier to maintain, extend, and reuse.

## Project Structure

```text
components/
│
├── app/             # Application logic
├── communication/   # UART and robot communication
├── core/            # Core system functionality
├── network/         # Wi-Fi, mDNS, TCP, WebSocket...
├── protocol/        # Robot protocol implementation
└── utils/           # Utility functions

main/
└── app_main.c       # Application entry point
```

## Third-Party Components

### mDNS

Documentation:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/mdns.html

The component was added using the ESP-IDF Component Manager:

```bash
idf.py add-dependency espressif/mdns
```
