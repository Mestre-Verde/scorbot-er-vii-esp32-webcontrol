#if !defined(UTILS)
#define UTILS

#include <stdio.h>

// | Teclas                 | Byte (dec) | HEX    | Nome       | Descrição                              |
// | ---------------------- | ---------- | ------ | ---------- | -------------------------------------- |
// | `Ctrl+A`               | 1          | `0x01` | SOH        | Start of Heading                       |
// | `Ctrl+B`               | 2          | `0x02` | STX        | Start of Text                          |
// | `Ctrl+C`               | 3          | `0x03` | ETX        | End of Text                            |
// | `Ctrl+D`               | 4          | `0x04` | EOT        | End of Transmission                    |
// | `Ctrl+E`               | 5          | `0x05` | ENQ        | Enquiry                                |
// | `Ctrl+F`               | 6          | `0x06` | ACK        | Acknowledge                            |
// | `Ctrl+G`               | 7          | `0x07` | BEL        | Bell                                   |
// | `Ctrl+H` / `Backspace` | 8          | `0x08` | BS         | Backspace                              |
// | `Ctrl+I` / `Tab`       | 9          | `0x09` | HT         | Horizontal Tab                         |
// | `Ctrl+J` / `Enter`*    | 10         | `0x0A` | LF         | Line Feed                              |
// | `Ctrl+K`               | 11         | `0x0B` | VT         | Vertical Tab                           |
// | `Ctrl+L`               | 12         | `0x0C` | FF         | Form Feed                              |
// | `Enter`* / `Ctrl+M`    | 13         | `0x0D` | CR         | Carriage Return                        |
// | `Ctrl+N`               | 14         | `0x0E` | SO         | Shift Out                              |
// | `Ctrl+O`               | 15         | `0x0F` | SI         | Shift In                               |
// | `Ctrl+P`               | 16         | `0x10` | DLE        | Data Link Escape                       |
// | `Ctrl+Q`               | 17         | `0x11` | DC1 / XON  | Device Control 1 — retomar transmissão |
// | `Ctrl+R`               | 18         | `0x12` | DC2        | Device Control 2                       |
// | `Ctrl+S`               | 19         | `0x13` | DC3 / XOFF | Device Control 3 — pausar transmissão  |
// | `Ctrl+T`               | 20         | `0x14` | DC4        | Device Control 4                       |
// | `Ctrl+U`               | 21         | `0x15` | NAK        | Negative Acknowledge                   |
// | `Ctrl+V`               | 22         | `0x16` | SYN        | Synchronous Idle                       |
// | `Ctrl+W`               | 23         | `0x17` | ETB        | End of Transmission Block              |
// | `Ctrl+X`               | 24         | `0x18` | CAN        | Cancel                                 |
// | `Ctrl+Y`               | 25         | `0x19` | EM         | End of Medium                          |
// | `Ctrl+Z`               | 26         | `0x1A` | SUB        | Substitute                             |
// | `Ctrl+[`               | 27         | `0x1B` | ESC        | Escape                                 |
// | `Ctrl+\`               | 28         | `0x1C` | FS         | File Separator                         |
// | `Ctrl+]`               | 29         | `0x1D` | GS         | Group Separator                        |
// | `Ctrl+^`               | 30         | `0x1E` | RS         | Record Separator                       |
// | `Ctrl+_`               | 31         | `0x1F` | US         | Unit Separator                         |
// | `Space`                | 32         | `0x20` | SP         | Space                                  |
// | `Delete`\*             | 127        | `0x7F` | DEL        | Delete                                 |

#define SOH  0x01  // Start of Heading
#define STX  0x02  // Start of Text
#define ETX  0x03  // End of Text
#define EOT  0x04  // End of Transmission
#define ENQ  0x05  // Enquiry
#define ACK  0x06  // Acknowledge
#define BEL  0x07  // Bell
#define BS   0x08  // Backspace
#define HT   0x09  // Horizontal Tab
#define LF   0x0A  // Line Feed
#define VT   0x0B  // Vertical Tab
#define FF   0x0C  // Form Feed
#define CR   0x0D  // Carriage Return
#define SO   0x0E  // Shift Out
#define SI   0x0F  // Shift In
#define DLE  0x10  // Data Link Escape
#define XON  0x11  // DC1 — Resume transmission
#define DC2  0x12  // Device Control 2
#define XOFF 0x13  // DC3 — Pause transmission
#define DC4  0x14  // Device Control 4
#define NAK  0x15  // Negative Acknowledge
#define SYN  0x16  // Synchronous Idle
#define ETB  0x17  // End of Transmission Block
#define CAN  0x18  // Cancel
#define EM   0x19  // End of Medium
#define SUB  0x1A  // Substitute
#define ESC  0x1B  // Escape
#define FS   0x1C  // File Separator
#define GS   0x1D  // Group Separator
#define RS   0x1E  // Record Separator
#define US   0x1F  // Unit Separator
#define SP   0x20  // Space
#define DEL  0x7F  // Delete

// conversor ASCII -> hexadecimals
void PrintBytesHEX(const char buffer[], size_t len);

#endif // UTILS
