#include "FS.h"
#include "define.h"
#include "constants.h"


FS *filesystem = &SPIFFS;
VSTUP_t DIN[pocetDIN_celkovo];

u8 LAN_MAC[6];
byte mac[][NUMBER_OF_MAC] =
    {
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14},
};