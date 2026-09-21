# ESP32 API for WebSocket Control of the Scorbot-ER VII

## For Schools 🏫

This project was created to give a second life to the classic **Scorbot-ER VII** educational robot. The goal is to modernize the robot while preserving its educational value, making it easier to use in classrooms and laboratories.

---

# ESP32 API 📁

This project is developed using the **ESP-IDF** framework. PlatfomrIO with Arduino workspace was used in the previus versions (V2.2.0 and previus).

The codebase follows a modular architecture based on ESP-IDF components, making it easier to maintain, extend, and reuse.

## Project Structure

```text
components/
│
├── API/             # Application logic
├── communication/   # UART and robot communication
├── network/         # Wi-Fi, mDNS, TCP, WebSocket...
└── utils/           # Utility functions

main/
│
├── core/            # Core system functionality
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

# Estrutura

```txt
                            ┌──────────────┐
                            │              │
 Mainframe ◄─ RS232/UART ─► │    ESP32     │
                            │              │
                            └──────┬───────┘
                                   │
                       ┌───────────┴───────────┐
                       │                       │
                     UART                    Wi-Fi
                       │                       │
                       ▼                       ▼
               PC / ESP Terminal      Students / Teachers
                     Debug                  API usage
```
