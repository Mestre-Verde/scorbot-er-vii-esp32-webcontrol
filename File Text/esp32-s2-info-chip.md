# chipe esp32-S2

## Block diagram chip
![Block diaram](Images/Block_Diagram.png)

## WIFI
  - IEEE 802.11 b/g/n-compliant• Automatic Beacon monitoring (hardware TSF)
  - Supports 20 MHz, 40 MHz bandwidth in 2.4 GHz band
  - 4 × virtual Wi-Fi interfaces
  - Simultaneous support for Infrastructure Station,SoftAP, and Promiscuous modes and performs a scan, the SoftAP channel will change along with the Station channel.
  > Note that when ESP32-S2 is in Station mode
  - Single-band 1T1R mode with data rate up to 150 Mbps
  - WMM
  - TX/RX A-MPDU, RX A-MSDU
  - Antenna diversity
  - Immediate Block ACK
  - 802.11mc FTM
  - Fragmentation and defragmentation

## CPU and Memory
  - Xtensa® single-core 32-bit LX7 microprocessor,up to 240 MHz
  - 16 KB SRAM in RTC
  - Embedded flash and PSRAM (see details in Chapter 1: ESP32-S2 Series Comparison)
  - CoreMark score: 1 core at 240 MHz: 472.73 CoreMark; 1.97 CoreMark/MHz
  - SPI/QSPI/OSPI supports multiple flash and external RAM chips
  - 128 KB ROM
  - Access to flash accelerated by cache
  - Supports flash in-Circuit Programming (ICP)
  - 320 KB SRAM
  - 40MHz crystal oscillator

## Advanced Peripheral Interfaces
  - 43 × programmable GPIOs
  - 4 × pulse counters (4 contadores físicos)

  Digital interfaces:
  - 4 × SPI
  - 1 × I2S
  - 2 × I2C
  - 2 × UART
  - 4 × pulse counters
  - Digital interfaces:– 1 × full-speed USB OTG
  - 1 × DVP 8/16 camera interface, implemented using the hardware resources of I2S
  - 1 × LCD interface (8-bit serial RGB/8080/6800), implemented using the hardware resources of I2S
  - 1 × LCD interface (8/16/24-bit parallel),implemented using the hardware resources of I2S
  - DMA controller
  - 1 × TWAI controller compatible with ISO11898-1 (CAN Specification 2.0)
  - 14 × touch sensing GPIOs
  - 1 × temperature sensor

## Timers
  - 1 × 64-bit general-purpose timer
  - 1 × 64-bit system timer
  - 3 × watchdog timers
  - 1 × super watchdog timer
  - 1 × XTAL32K watchdog timer
  
## Secure boot
  - Flash encryption
  - 4096-bit OTP, up to 1792 bits for users
  - Cryptographic hardware acceleration:
    - Hash (FIPS PUB 180-4)
    - HMAC
    - Random Number Generator (RNG)
    - RSA
    - AES-128/192/256 (FIPS PUB 197)
    - Digital signature

# Board info

  - Modelo: ESP32-S2-DevKitC-1U-N8
  - Flash externa: 8MB
  - PSRAM: 0MB.
  - External antenna conector
  
## Block diagram Module
![Block diaram](Images/module_diagram_block.png)

## Module PINOUT

![Board Pinouts](Images/PINOUT.png)


# 

# RESET

```text
Power ON
```

O chip arrancou por alimentação normal.

Outras causas possíveis seriam:

* watchdog
* software reset
* brownout
* wakeup deep sleep
* crash/panic

Portanto aqui está saudável.

---

# CHIP INFO

```text
Model: ESP32
```

Aqui o teu código imprime sempre `"ESP32"` fixo.
Na realidade isto é enganador porque o teu chip é um S2.

Podes melhorar depois usando:

```cpp
ESP.getChipModel()
```

---

```text
Cores: 1
```

O ESP32-S2 só tem:

* 1 núcleo Xtensa LX7

Enquanto:

* ESP32 clássico → 2 núcleos LX6
* S3 → 2 núcleos LX7
* C3 → 1 núcleo RISC-V

---

```text
Features: WiFi
```

O S2:

* tem Wi-Fi
* NÃO tem Bluetooth/BLE

Por isso só aparece WiFi.

---

```text
Revision: 0
```

É revisão de silício.

Revision 0 normalmente significa:

* primeira revisão física do chip

Às vezes revisões antigas têm erratas/bugs conhecidos.

---

```text
CPU Frequency: 240 MHz
```

O núcleo está no clock máximo.

No S2:

* 80 MHz
* 160 MHz
* 240 MHz

---

# MEMÓRIA

---

```text
Flash Size: 8 MB
```

A tua placa tem:

* 8 megabytes de flash SPI externa

Isto é onde ficam:

* firmware
* filesystem
* OTA
* NVS
* etc

Boa capacidade.

---

```text
Flash Speed: 80 MHz
```

A flash está a comunicar a:

* 80 MHz

Normal para ESP32 modernos.

---

```text
Sketch Size: 269872 bytes
```

O firmware ocupa:

* ~270 KB

Muito pequeno ainda.

---

```text
Free Sketch Space: 1310720 bytes
```

Ainda tens:

* ~1.3 MB livres
  para firmware OTA.

---

# HEAP (RAM dinâmica)

---

```text
Heap Total: 247980 bytes
```

RAM disponível para malloc/new.

---

```text
Heap Free: 227576 bytes
```

Tens ~227 KB livres.

Muito bom.

---

```text
Heap Min Free: 222604 bytes
```

Menor valor já atingido desde boot.

Isto é MUITO útil para:

* debug
* leaks
* dimensionamento

---

```text
Largest Free Block: 196596 bytes
```

Maior bloco contínuo possível.

Importante porque:

* podes ter heap livre
* mas fragmentada

Aqui está excelente.

---

# PSRAM

```text
PSRAM Size: 0 bytes
```

A tua board:

* NÃO tem PSRAM externa

Algumas boards têm:

* 2 MB
* 4 MB
* 8 MB

Muito útil para:

* imagens
* buffers
* áudio
* AI
* web servers

---

# SDK

```text
SDK Version: v4.4.7-dirty
```

Usas:

* ESP-IDF 4.4.7

`dirty` significa:

* houve modificações locais no build

É normal no core Arduino ESP32.

---

```text
Arduino Core Version: 131089
```

Versão interna codificada do Arduino core.

---

# MAC ADDRESS

```text
70:04:1D:FA:20:1C
```

MAC físico único do Wi-Fi.

Cada ESP tem um único.

---

# CHIP UNIQUE ID

```text
Chip ID: 1C20FA1D0470
```

ID único derivado das eFuses.

Muito usado para:

* licenciamento
* identificação
* pairing
* serial numbers

---

# TEMPERATURA

```text
Temperature: 28.45 °C
```

Sensor interno do S2.

IMPORTANTE:

* não é super preciso
* mede mais o die interno
* aquece com CPU/Wi-Fi

Mas é ótimo para:

* proteção térmica
* tendência
* monitorização

---

# FREE RTOS

---

```text
Running Core: 0
```

Estás no único core existente.

---

```text
Free Stack Loop Task: 6464 bytes
```

A task `loop()` ainda tem:

* ~6.4 KB de stack livre

Muito saudável.

Se isto descer demasiado:

* stack overflow
* crashes estranhos
* corrupção

---

# POWER

```text
Hall sensor not available
```

Correto para S2.

---

# LIVE INFO

---

```text
Uptime: 5 seconds
```

Tempo desde boot.

---

```text
Free Heap: 227312 bytes
```

Heap praticamente intacta.

Sem leaks aparentes.

---

```text
WiFi RSSI: 0
```

Isto significa:

* Wi-Fi não conectado

Quando ligado:

* -30 dBm → excelente
* -50 → muito bom
* -70 → fraco
* -90 → quase morto

---

Conclusão geral do estado do chip:

* saudável
* sem leaks
* sem fragmentação séria
* temperatura ótima
* stack confortável
* flash boa
* RAM suficiente
* sem PSRAM
* single core
* Wi-Fi ready

Para projetos de:

* APIs
* WebSocket
* controlo robótico
* sensores
* automação
* web interface

esse S2 está perfeitamente utilizável.
