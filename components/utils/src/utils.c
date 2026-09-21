/*
utils functions.
*/
#include <stdio.h>

void PrintBytesHEX(const char buffer[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X ", (unsigned char)buffer[i]);
    }
    fflush(stdout);
}