#ifndef __HelpFunction_H
#define __HelpFunction_H

#include <Arduino.h>
//#include"constants.h"


#define ComDebug(x) Serial.print(x);
#define ComDebugln(x) Serial.println(x);

char **split(char **argv, int *argc, char *string, const char delimiter, int allowempty);

String ConvetWeekDay_UStoCZ(tm *timeInfoPRT);
String ConvetWeekDay_UStoSK(tm *timeInfoPRT);
bool SkontrolujCiJePovolenyDenvTyzdni(u8 Obraz, tm *timeInfoPRT);
uint32_t readADC_Avg(int ADC_Raw);
static bool Input_digital_filtering(VSTUP_t *input_struct, uint16_t filterCas);
void ScanInputs(void);
void System_init(void);
int8_t NacitajEEPROM_setting(void);

int getIpBlock(int index, String str);
String ipToString(IPAddress ip);
IPAddress str2IP(String str);

String handle_LenZobraz_IP_setting(void);
String handle_Zadavanie_IP_setting(void);

void OdosliStrankeVytapeniData(void);
void TCP_debugMsg(String sprava);
u16 readADC_Avg(void);
uint8_t VypocitajSumuBuffera(uint8_t *buffer, uint16_t kolko);
uint8_t KontrolaSumyBuffera(uint8_t *buffer, uint16_t kolko);
bool KontrolujBufferZdaObsaujeJSONdata(char JSONbuffer[]);
uint16_t Read_u16_Value(char *buff);
uint32_t Read_u32_Value(char *buff);
int32_t Read_32_Value(char *buff);
u64 Read_u64_Value(char *buff);
void Swap_float(float *v);
float Read_Float_Value(char *buff);
void float2Bytes(float val, uint8_t *bytes_array);
void Double2Bytes(double val, uint8_t *bytes_array);
bool OdosliZaznamDosocketu(LOGBUFF_t *logBuffStruc);
bool UlozZaznam(LOGBUFF_t *logBuffStruc);
String VyberZaznam(LOGBUFF_t *logBuffStruc, bool zmazZaznam);
u16 VratPocetZaznamu(LOGBUFF_t *logBuffStruc);

void NacitajSuborzSD(void);
void UDPhandler(void);
#endif