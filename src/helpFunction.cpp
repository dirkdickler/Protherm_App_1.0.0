//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <Arduino.h>
#include "constants.h"
#include "Pin_assigment.h"
//#include "HelpFunction.h"
#include "define.h"
#include "constants.h"
#include "Ethernet3.h"
#include "main.h"
#include <EEPROM.h>

#include "SD.h"
#include "FS.h"
#include "SPIFFS.h"
#include "esp_log.h"
#include <EthernetUdp3.h>

EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
char ReplyBuffer[] = "000102030405 ProthermEnergy";       // a string to send back


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
    Udp.begin(9999); // toto musi byt az po  Ethernet.begin
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

int8_t NacitajEEPROM_setting(void)
{

	if (!EEPROM.begin(500))
	{
		log_i("Failed to initialise EEPROM");
		return -1;
	}

	log_i("Succes to initialise EEPROM");

	//EEPROM.writeString(EE_NazovSiete, "zadels\0");
	//EEPROM.writeLong(EE_Vin_gain_1,123);
	//EEPROM.commit();
	//EEPROM.readString(EE_NazovSiete, NazovSiete, 16);
	
	String slovo;
	slovo = String(EEPROM.readString(EE_NazovSiete));//, NazovSiete,16);//String(Ethernet.localIP());
	log_i("EEPROM nazov siete je vycitane: %s", slovo);
	//EEPROM.readBytes(EE_NazovSiete, NazovSiete, 16);

	if (NazovSiete[0] != 0xff) // ak mas novy modul tak EEPROM vrati prazdne hodnoty, preto ich neprepisem z EEPROM, ale necham default
	{
		String apipch = EEPROM.readString(EE_IPadresa); // "192.168.1.11";
		local_IP = str2IP(apipch);

		apipch = EEPROM.readString(EE_SUBNET);
		subnet = str2IP(apipch);

		apipch = EEPROM.readString(EE_Brana);
		gateway = str2IP(apipch);

		memset(NazovSiete, 0, sizeof(NazovSiete));
		memset(Heslo, 0, sizeof(Heslo));
		u8 dd = EEPROM.readBytes(EE_NazovSiete, NazovSiete, 16);
		u8 ww = EEPROM.readBytes(EE_Heslosiete, Heslo, 20);
		log_i("Nacitany nazov siete a heslo z EEPROM: %s  a %s\r\n", NazovSiete, Heslo);
		return 0;
	}
	else
	{
		log_i("EEPROM je este prazna, nachavma default hodnoty");
		return 1;
	}
}
String ipToString(IPAddress ip)
{
	String s = "";
	for (int i = 0; i < 4; i++)
		s += i ? "." + String(ip[i]) : String(ip[i]);
	return s;
}

int getIpBlock(int index, String str)
{
	char separator = '.';
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = str.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (str.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}

IPAddress str2IP(String str)
{

	IPAddress ret(getIpBlock(0, str), getIpBlock(1, str), getIpBlock(2, str), getIpBlock(3, str));
	return ret;
}



void UDPhandler(void)
{
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		Serial.print("Received packet of size ");
		Serial.println(packetSize);
		Serial.print("From ");
		IPAddress remote = Udp.remoteIP();
		for (int i = 0; i < 4; i++)
		{
			Serial.print(remote[i], DEC);
			if (i < 3)
			{
				Serial.print(".");
			}
		}
		Serial.print(", port ");
		Serial.println(Udp.remotePort());

		// read the packet into packetBufffer
		Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		Serial.println("Contents:");
		Serial.println(packetBuffer);

		// send a reply, to the IP address and port that sent us the packet we received
		Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
		Udp.write(ReplyBuffer);
		Udp.endPacket();
	}
}