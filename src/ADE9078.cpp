#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ADE9078.h"
#include "constants.h"
#include <SPI.h>

#define cas 4

// SPIClass *ADE_spi = NULL;
SPIClass ADE_spi(HSPI);
SPISettings ADE_SPI_settings(1000000, MSBFIRST, SPI_MODE3);

MERANIE_t meranie;

void ADE9078_init(void)
{
	log_i("ADE9078 inicializujem..");
	pinMode(ADE9078_CS, OUTPUT);
	ADE9078_CS_HIGH();
	// ADE_spi = new SPIClass(HSPI);
	ADE_spi.setFrequency(5000000);
	ADE_spi.begin(ADE9078_SCK, ADE9078_MISO, ADE9078_MOSI, -1);
}
static void ADE9078_delay_us(uint32_t us)
{
	us *= 30;

	// __asm volatile(" mov r0, %[us] \n\t"
	//              "1: subs r0, #1 \n\t"
	//              " bhi 1b \n\t"
	//              :
	//              : [us] "r" (us)
	//              : "r0");
}

uint16_t ADE9078_GetVersion(void) // Cip ADE9078 musi vratit 0x0040!!!
{
	static volatile uint16_t ver;
	uint8_t ptrDataTx[6], ptrDataRx[6];
	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = ADDR_VERSION;
	ver <<= 4;
	sbi(ver, 3); // READ
	ptrDataTx[0] = (ver >> 8);
	ptrDataTx[1] = (ver & 0xff);

	// HAL_SPI_TransmitReceive(&hspi1, ptrDataTx, ptrDataRx, 6, 1000);
	ADE_spi.transferBytes(ptrDataTx, ptrDataRx, 6);
	ver = ptrDataRx[2];
	ver <<= 8;
	ver += ptrDataRx[3];
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
	return ver;
}

uint8_t ADE9078_SpiSendData(uint8_t dat)
{
	uint8_t ptrDataTx[2], ptrDataRx[2];
	ptrDataTx[0] = dat;
	// HAL_SPI_TransmitReceive(&hspi1, ptrDataTx, ptrDataRx, 1, 1000);
	return ptrDataRx[0];
}

void ADE9078_send_swSPI_byte(uint8_t co)
{
}

uint8_t ADE9078_read_swSPI_byte(void)
{
	uint8_t co = 0;

	return co;
}

uint16_t ADE9078_read_register(uint16_t adress)
{

	return 0;
}

void ADE9078_write_register(uint16_t adress, uint16_t hodnota)
{
}

uint16_t ADE9078_chksum_calc(uint8_t reg_from, uint8_t kolko_registru)
{
	uint8_t suma_HiByte = 0;
	uint8_t suma_LoByte = 0;
	uint16_t suma;

	return suma;
}

void ADE9078_Wr16(uint16_t adresa, uint16_t val2Wr)
{
	uint16_t ver;
	uint8_t ptrDataTx[4];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	cbi(ver, 3); // Write
	ptrDataTx[0] = (u8)(ver >> 8);
	ptrDataTx[1] = (u8)(ver & 0xff);
	ptrDataTx[2] = (u8)(val2Wr >> 8);
	ptrDataTx[3] = (u8)(val2Wr & 0xff);
	ADE_spi.transferBytes(ptrDataTx, NULL, 4);
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
}

void ADE9078_Wr32(uint16_t adresa, uint32_t val2Wr)
{
	uint16_t ver;
	uint8_t ptrDataTx[6];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	cbi(ver, 3); // Write
	ptrDataTx[0] = (u8)(ver >> 8);
	ptrDataTx[1] = (u8)(ver & 0xff);
	ptrDataTx[2] = (u8)(val2Wr >> 24);
	ptrDataTx[3] = (u8)(val2Wr >> 16);
	ptrDataTx[4] = (u8)(val2Wr >> 8);
	ptrDataTx[5] = (u8)(val2Wr & 0xff);
	ADE_spi.transferBytes(ptrDataTx, NULL, 6);
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
}

uint16_t ADE9078_Rd_u16(uint16_t adresa)
{
	uint16_t ver;
	uint8_t ptrDataTx[4], ptrDataRx[4];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	sbi(ver, 3); // READ
	ptrDataTx[0] = ver >> 8;
	ptrDataTx[1] = ver & 0xff;
	ADE_spi.transferBytes(ptrDataTx, ptrDataRx, 4);
	ver = ptrDataRx[2];
	ver <<= 8;
	ver += ptrDataRx[3];
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
	return ver;
}

int16_t ADE9078_Rd_i16(uint16_t adresa)
{
	int16_t ver;
	uint8_t ptrDataTx[4], ptrDataRx[4];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	sbi(ver, 3); // READ
	ptrDataTx[0] = ver >> 8;
	ptrDataTx[1] = ver & 0xff;
	ADE_spi.transferBytes(ptrDataTx, ptrDataRx, 4);
	ver = ptrDataRx[2];
	ver <<= 8;
	ver += ptrDataRx[3];
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
	return ver;
}

uint32_t ADE9078_Rd_u32(uint16_t adresa)
{
	uint32_t ver;
	uint8_t ptrDataTx[6], ptrDataRx[6];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	sbi(ver, 3); // READ
	ptrDataTx[0] = ver >> 8;
	ptrDataTx[1] = ver & 0xff;
	ADE_spi.transferBytes(ptrDataTx, ptrDataRx, 6);
	ver = ptrDataRx[2];
	ver <<= 8;
	ver += ptrDataRx[3];
	ver <<= 8;
	ver += ptrDataRx[4];
	ver <<= 8;
	ver += ptrDataRx[5];
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
	return ver;
}

int32_t ADE9078_Rd_i32(uint16_t adresa)
{
	int32_t ver;
	uint8_t ptrDataTx[6], ptrDataRx[6];

	ADE_spi.beginTransaction(ADE_SPI_settings);
	ADE9078_CS_LOW();
	ver = adresa;
	ver <<= 4;
	sbi(ver, 3); // READ
	ptrDataTx[0] = ver >> 8;
	ptrDataTx[1] = ver & 0xff;
	ADE_spi.transferBytes(ptrDataTx, ptrDataRx, 6);
	ver = ptrDataRx[2];
	ver <<= 8;
	ver += ptrDataRx[3];
	ver <<= 8;
	ver += ptrDataRx[4];
	ver <<= 8;
	ver += ptrDataRx[5];
	ADE9078_CS_HIGH();
	ADE_spi.endTransaction();
	return ver;
}
