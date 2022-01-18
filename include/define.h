#ifndef INC_DEFINE_H
#define INC_DEFINE_H

#define ETHERNET3_pocet_SOCK_NUM 6 // USE_ETHERNET3  Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
#define WDT_TIMEOUT 5

#define input1 0
#define input2 1
#define input3 2
#define input4 3
#define input_SDkarta 5

#define pocetDIN 4
#define pocetDIN_celkovo 6  // DIN + SD CD pin , !!! ale daj o +1 lebo nevim preco ho to prepisovalo davalo do neho RTC sek :-)
#define filterTime_DI 3     // 10ms loop
#define filterTime_SD_CD 10 // 10ms loop

#define rozsah_20A_1F 1
#define rozsah_20A_3F 2
#define rozsah_50A_3F 3
#define rozsah_100A_3F 4

#define TCPsocket 7

#define WDT_TIMEOUT 5
#define firmware "ver20220118_001"

// EEPROM adrese
#define EE_MAC_LAN 00               // 6 bytes     6+16+16+16+16+20+2+2
#define EE_IPadresa 06              // 16 bytes  
#define EE_SUBNET 22                // 16 bytes
#define EE_Brana 38                 // 16 bytes
#define EE_NazovSiete 54            // 16 bytes
#define EE_Heslosiete 70            // 20 bytes
#define EE_citacZapisuDoEEPORM 90   // 2 bytes
#define EE_citac2_ZapisuDoEEPORM 92 // 2 bytes
#define EE_dalsi 94

#define EE_rozsah_Prud 99     // 1byte
#define EE_Vin_offset_1 100   // 4bytes
#define EE_Vin_gain_1 104     // 4bytes
#define EE_Vin_offset_2 108   // 4bytes
#define EE_Vin_gain_2 112     // 4bytes
#define EE_Vin_offset_3 116   // 4bytes
#define EE_Vin_gain_3 120     // 4bytes
#define EE_Iin_offset_1 124   // 4bytes
#define EE_Iin_gain_1_20A 128 // 4bytes
#define EE_Iin_offset_2 132   // 4bytes
#define EE_Iin_gain_2_20A 136 // 4bytes
#define EE_Iin_offset_3 140   // 4bytes
#define EE_Iin_gain_3_20A 144 // 4bytes

#define EE_Iin_gain_1_50A 148  // 4bytes
#define EE_Iin_gain_2_50A 152  // 4bytes
#define EE_Iin_gain_3_50A 156  // 4bytes
#define EE_Iin_gain_1_100A 160 // 4bytes
#define EE_Iin_gain_2_100A 164 // 4bytes
#define EE_Iin_gain_3_100A 168 // 4bytes

#define EE_zacateKaret_1 200
#define EE_zacateKaret_2 1300 // EE_zacateKaret + 100*11tj 1300

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 0

#define sbi(adr, bitt) (adr |= (1 << bitt))
#define cbi(adr, bitt) (adr &= ~(1 << bitt))
#define isbit(adr, bitt) (adr & (1 << bitt))
#define HI(ii) (ii >> 8)
#define LO(ii) (ii & 0xff)

#define USE_LITTLEFS false
#define USE_SPIFFS true

#if USE_LITTLEFS
// LittleFS has higher priority
#include "FS.h"

// The library will be depreciated after being merged to future major Arduino esp32 core release 2.x
// At that time, just remove this library inclusion
#include <LITTLEFS.h> // https://github.com/lorol/LITTLEFS

FS *filesystem = &LITTLEFS;
#define CurrentFileFS "LittleFS"
#define FileFS LITTLEFS

#ifdef USE_SPIFFS
#undef USE_SPIFFS
#endif

#define USE_SPIFFS false
#elif USE_SPIFFS
#include "FS.h"
#include <SPIFFS.h>

// FS *filesystem = &SPIFFS;
#define FileFS SPIFFS
#define CurrentFileFS "SPIFFS"
#endif

#define ETHERNET_USE_ESP32
#define BOARD_TYPE "ESP32"
#define BOARD_NAME BOARD_TYPE

#define USE_ETHERNET_WRAPPER false
// Use true  for ENC28J60 and UIPEthernet library (https://github.com/UIPEthernet/UIPEthernet)
// Use false for W5x00 and Ethernetx library      (https://www.arduino.cc/en/Reference/Ethernet)

#define USE_UIP_ETHERNET false

// Only one if the following to be true
#define USE_ETHERNET false
#define USE_ETHERNET2 false
#define USE_ETHERNET3 true
#define USE_ETHERNET_LARGE false
#define USE_ETHERNET_ESP8266 false
#define USE_ETHERNET_ENC false
#define USE_CUSTOM_ETHERNET false

#define SHIELD_TYPE "W5x00 using Ethernet3 Library"

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC 20

#endif // define_h
