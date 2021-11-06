//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <Arduino.h>
//#include"constants.h"
#include "Pin_assigment.h"
//#include "HelpFunction.h"
#include "define.h"
#include "constants.h"
#include "Ethernet3.h"
#include"main.h"

#include "SD.h"
#include "FS.h"
#include <SPIFFS.h>
#include "esp_log.h"

void System_init(void)
{
	Serial.println("Nastavujem DEBUG LOGs  priklady v pliformIO ini musis mat nastavene nejaku uroven -DCORE_DEBUG_LEVEL=5 ");
	log_i("Zacatek funkcie..");
	// ESP_LOGD("EXAMPLE", "This doesn't show");
	// ESP_LOGI("","Func:System_init  begin..");
	// ESP_LOGI("", " TESTIK Ix storage usedd: %ld/%ld", 123, 124);
	// log_w("TESTIK Ix storage usedd: %f/%f", 12.3, 12.4);
	// ESP_LOGW("", "TESTIK Wx storage usedd: %ld/%ld", 23, 24);

	DIN[input1].pin = DI1_pin;
	DIN[input2].pin = DI2_pin;
	DIN[input3].pin = DI3_pin;
	DIN[input4].pin = DI4_pin;
	DIN[input_SDkarta].pin = SD_CD_pin;

	pinMode(ADC_gain_pin, OUTPUT_OPEN_DRAIN);
	pinMode(SD_CS_pin, OUTPUT);
	pinMode(WIZ_CS_pin, OUTPUT);
	pinMode(WIZ_RES_pin, OUTPUT);

	// WizChip_RST_HI();
	// WizChip_CS_HI();

	pinMode(DI1_pin, INPUT_PULLUP);
	pinMode(DI2_pin, INPUT_PULLUP);
	pinMode(DI3_pin, INPUT_PULLUP);
	pinMode(DI4_pin, INPUT_PULLUP);
	pinMode(SD_CD_pin, INPUT_PULLUP);
	// pinMode(WIZ_INT_pin, INPUT_PULLUP); //neni
	pinMode(Joy_up_pin, INPUT_PULLUP);
	pinMode(Joy_dn_pin, INPUT_PULLUP);
	pinMode(Joy_Butt_pin, INPUT_PULLUP);
	pinMode(Joy_left_pin, INPUT_PULLUP);
	pinMode(Joy_right_pin, INPUT_PULLUP);

	// RTC_Date Pccc;
	// Wire.begin(SCL_pin, SDA_pin);
	// PCFrtc.begin();
	// PCFrtc.setDateTime(2019, 4, 1, 12, 33, 59);
	// Pccc = PCFrtc.getDateTime();
	// rtc.setTime(Pccc.second, Pccc.minute, Pccc.hour, Pccc.day, Pccc.month, Pccc.year); // 17th Jan 2021 15:24:30

	// NaplnWizChipStrukturu();

	// SDSPI.setFrequency(10000000); // nezabudni ze pri SD.begin(SD_CS_pin, SDSPI,10000000)) budes menit fre na hodnotu v zavorkach
	//  SDSPI.setClockDivider(SPI_CLOCK_DIV2);
	// SDSPI.begin(SD_sck, SD_miso, SD_mosi, -1);

	// WizChip_Reset();
	// WizChip_Config(&eth);

	SPI.setFrequency(15000000);
	SPI.begin(SD_sck, SD_miso, SD_mosi, -1);
	pinMode(SD_CS_pin, OUTPUT);
	pinMode(SD_CD_pin, INPUT_PULLUP);

	// if (!SD.begin(SD_CS_pin, SPI))
	// {
	// 	log_i("Card Mount begin: ERROR");
	// }
	// else
	// {
	// 	log_i("Card Mount begin: OK");
	// }

	if (!SPIFFS.begin(true))
	{
		log_i("SPIFS begin:: ERROR");
	}
	else
	{
		log_i("SPIFS begin:: OK");
	}

	// File file = SPIFFS.open("/page2.html");
	// if (!file)
	// {
	// 	Serial.println("Failed to open file for reading");
	// 	return;
	// }

	// Serial.println("File Content:");
	// while (file.available())
	// {
	// 	Serial.write(file.read());
	// }
	// file.close();

	log_i("Starting AdvancedWebServer on %s ", BOARD_NAME);
	log_i(" with %s", SHIELD_TYPE);
	log_i("ETHERNET_WEBSERVER_VERSION si pozri v H subore");

	Ethernet.setRstPin(WIZ_RES_pin); // 14
	Ethernet.setCsPin(WIZ_CS_pin);
	Ethernet.hardreset();
	Ethernet.init(ETHERNET3_pocet_SOCK_NUM);
	uint16_t index = millis() % NUMBER_OF_MAC;
	// Use Static IP
	// Ethernet.begin(mac[index], ip);
	Ethernet.begin(mac[6], local_IP, subnet, gateway, primaryDNS);

	log_i("Using mac index = %u", index);
	String slovo;
	slovo = String(Ethernet.localIP());
	log_i("Connected! IP address: %s", slovo);

	log_i("Konec funkcie..");
}

void NacitajSuborzSD(void)
{
	if (!SD.begin(SD_CS_pin, SPI))
	{
		log_i("Card Mount begin: ERROR");
		return;
	}
	else
	{
		log_i("Card Mount begin: OK");
	}

	uint8_t cardType = SD.cardType();
	if (cardType == CARD_NONE)
	{
		log_i("No SD card attached");
		return;
	}

	if (cardType == CARD_MMC)
	{
		log_i("SD Card Type: MMC");
	}
	else if (cardType == CARD_SD)
	{
		// Serial.println("SDSC");
		log_i("SD Card Type: SDSC");
	}
	else if (cardType == CARD_SDHC)
	{
		// Serial.println("SDHC");
		log_i("SD Card Type: SDHD");
	}
	else
	{
		// Serial.println("UNKNOWN");
		log_i("SD Card Type: neznama");
	}

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	log_i("SD Card Size: %lluMB", cardSize);
	// Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
	log_i("Total space: %lluMB", SD.totalBytes() / (1024 * 1024));
	log_i("Used space: %lluMB", SD.usedBytes() / (1024 * 1024));

	u8 loo = 0;
	File profile;
	do
	{
		profile = SD.open("/aaa.txt", FILE_READ); /* code */
		if (!profile)
		{
			loo++;
			log_i("Open file vratilo profile %i", profile);
		}
		else
		{
			loo = 5;
		}
	} while (loo < 5);

	Serial.printf("Velkost subora je :%lu\r\n", profile.size());
	if (!profile)
	{
		log_i("Opening file to read failed");
		profile.close();
	}
	else
	{
		log_i("Obsah subora po nacitani je:\n");

		while (profile.available())
		{
			while (profile.available())
			{
				Serial.write(profile.read());
			}

			log_i("konec citania subora");
			log_i("================");
		}
		profile.close();
	}
}
