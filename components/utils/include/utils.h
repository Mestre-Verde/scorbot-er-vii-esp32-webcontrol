#if !defined(UTILS)
#define UTILS

#include <stdio.h>

#define XON 0x11  // Represents the byte "DC1".
#define XOFF 0x13 // Represents the byte "DC3".

// conversor ASCII -> hexadecimals
void PrintBytesHEX(const char buffer[], size_t len);

#endif // UTILS
