#include <Arduino.h>
#include <esp_task_wdt.h>
#include "define.h"
#include <SPI.h>
#include "Ethernet3.h"
#include <EthernetWebServer.h>
#include <utility\socket.h>
#include "utility\w5500.h"
#include "FS.h"
#include "SD.h"
#include <Ticker.h> 
#include "index.html"
#include <Arduino_JSON.h>
#include <EEPROM.h> 
#include "main.h" 
#include "constants.h"
#include "HelpFunction.h"
#include "Pin_assigment.h"
#include "esp_log.h" 
#include "ADE9078.h"

//#include <TaskScheduler.h>

bool GetData_flag = false;
// Task t1(1050, TASK_FOREVER, &t1_MAIN);
// Task t2(20, TASK_FOREVER, &t2_ethTask); 
// Scheduler runner;
TaskHandle_t htask1;  
TaskHandle_t htask2;
TaskHandle_t Task_handleADE9078;

const char *www_username = "qqq";
const char *www_password = "1234";

IPAddress local_IP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

EthernetWebServer server_LAN(8080);

char NazovSiete[30];
char Heslo[30];

Ticker timer_10ms(Loop_10ms, 100, 0, MILLIS);
static JSONVar myObject, AjaxObjekt;
static String jsonString;

FLAGS_t flg;

float citac = 0;
; // int reqCount = 0; // number of requests received

void Loop_10ms()
{
  citac++;
}

void handleNotFound()
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += server_LAN.uri();
  if (server_LAN.uri() == "/data.txt")
  {

    server_LAN.setContentLength(CONTENT_LENGTH_UNKNOWN); // https://bleepcoder.com/arduino/226633148/server-send-fails-to-send-strings-html-greater-than-6600
    server_LAN.send(200, "text/html", "xx");
    for (uint16_t i = 0; i < 300; i++)
    {
      server_LAN.sendContent("123451234512345123451234512345123451234512345123451234512345123451234512345123451234512345123451234512345123456\r\n"); /* code */ /* code */
    }
    server_LAN.sendContent("KOnec");
    return;
  }
  message += F("\nMethod: ");
  message += (server_LAN.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server_LAN.args();
  message += F("\n");

  for (uint8_t i = 0; i < server_LAN.args(); i++)
  {
    message += " " + server_LAN.argName(i) + ": " + server_LAN.arg(i) + "\n";
  }

  server_LAN.send(404, F("text/plain"), message);
}

void testFunct()
{
  char locBuff[2048];
  uint32_t index = 0;

  log_i("Desel pozadavek na main");
  log_i("Main ma delku %u", strlen(DebugLog_html)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));
  server_LAN.setContentLength(strlen(DebugLog_html));

  memset(locBuff, 0, sizeof(locBuff));
  server_LAN.send(200, "text/html", locBuff);
  u32 kolkoPoslnaych = 0;
  u32 velkostStranek = strlen(DebugLog_html);
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], velkostStranek);
      server_LAN.sendContent(locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], zostava);
        server_LAN.sendContent(locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], 1000);
        server_LAN.sendContent(locBuff, 1000);
        kolkoPoslnaych += 1000;
      }
    }
  } while (kolkoPoslnaych < velkostStranek);

  // File file = SD.open("/aaa.txt", FILE_READ);
  //  server.streamFile(file, "text/html");
  //  file.close();
}

void hlavne()
{
  char locBuff[2048];
  uint32_t index = 0;

  log_i("Desel pozadavek na main");
  log_i("Main ma delku %u", strlen(DebugLog_html)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));
  server_LAN.setContentLength(strlen(DebugLog_html));

  memset(locBuff, 0, sizeof(locBuff)); 
  server_LAN.send(200, "text/html", locBuff);
  u32 kolkoPoslnaych = 0;   
  u32 velkostStranek = strlen(DebugLog_html);  
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], velkostStranek);
      server_LAN.sendContent(locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], zostava);
        server_LAN.sendContent(locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], 1000);
        server_LAN.sendContent(locBuff, 1000);
        kolkoPoslnaych += 1000;
      }
    }
  } while (kolkoPoslnaych < velkostStranek);
}

void ReadSuborzSD()
{
  char locBuff[2048];
  uint32_t index = 0;

  log_i("Idenm citat suborDesel pozadavek na hlavne");
  server_LAN.send(200, F("text/plain"), F("subora nacitany"));

  NacitajSuborzSD();
}

char buff_extra[4001];

void setup(void)
{

  Serial.begin(115200);
  Serial.println("Spustam applikaciu...Beta 5");
  NacitajEEPROM_setting();
  System_init();

  log_i("Idem citat subore z SD karty po init web");
  NacitajSuborzSD();
  FuncServer_On();

  server_LAN.begin();

  timer_10ms.start();
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
  // runner.init();
  // Serial.println("Initialized scheduler");

  // runner.addTask(t1);
  // Serial.println("added t1");
  // runner.addTask(t2);
  // Serial.println("added t2");
  // t1.enable();
  // Serial.println("Enabled t1");
  // t2.enable();

  // assert(rc == pdPASS);

  xTaskCreatePinnedToCore(
      t1_MAIN, // Task function
      "task1", // Name
      6000,    // Stack size
      nullptr, // Parameters
      1,       // Priority
      &htask1, // handle
      0        // CPU
  );

  xTaskCreatePinnedToCore(
      t2_ethTask, // Task function
      "task2",    // Name
      6000,       // Stack size
      nullptr,    // Parameters
      1,          // Priority
      &htask2,    // handle
      0           // CPU
  );

  xTaskCreatePinnedToCore(
      Task_handle_ADE9078_Code, // Task function
      "Task_ADE9078",           // Name
      3000,                     // Stack size
      nullptr,                  // Parameters
      1,                        // Priority
      &Task_handleADE9078,      // handle
      0                         // CPU
  );
}

void loop(void)
{
  esp_task_wdt_reset();
  // runner.execute();
  timer_10ms.update();
  server_LAN.handleClient();

  delay(5);
}

void FuncServer_On(void)
{
  log_i("Zaciatok funkcie");
  server_LAN.on("/", HTTP_GET, []()
                {
              ET_LOGWARN(F("Dosel pozadavek na Handle ROot"));
              ET_LOGWARN1(F("TCP 0 status reg:"), w5500.readSnSR(7));
              ET_LOGWARN1(F("TCP1 status reg:"), w5500.readSnSR(6));

              server_LAN.send(200, F("text/plain"), F("Deska jede")); });

  server_LAN.on(F("/heslo"), []()
                {
              log_i("Doslo stranky / heslo");
              if (!server_LAN.authenticate(www_username, www_password))
              {
                log_i("Heslo dava return");
                return server_LAN.requestAuthentication();
              }
              log_i("Fajne heslo prebeglo odpovidam OK 200");
              server_LAN.send(200, F("text/plain"), F("Login OK \r\ntoto by ale asi mohel byt kuuuuuurna dlhy text")); });

  server_LAN.on(F("/pass"), []()
                {
    if (!server_LAN.authenticate(www_username, www_password))
    {
      return server_LAN.requestAuthentication();
    }
    server_LAN.send(200, F("text/plain"), F("Login OK")); });

  // server.on("/list", HTTP_GET, printDirectory);

  server_LAN.serveStatic("/page2", SPIFFS, "/page2.html");
  server_LAN.serveStatic("/page3", SPIFFS, "/page3.html");
  server_LAN.serveStatic("/vlajka1", SPIFFS, "/CanadaFlag_1.png");
  server_LAN.serveStatic("/vlajka2", SPIFFS, "/CanadaFlag_2.png");
  server_LAN.serveStatic("/vlajka3", SPIFFS, "/CanadaFlag_3.jpg");
  server_LAN.on("/main", HTTP_GET, testFunct);
  server_LAN.on("/hlavne", HTTP_GET, hlavne);

  server_LAN.on("/subor", HTTP_GET, ReadSuborzSD);

  server_LAN.on("/get", HTTP_GET, []()
                { 
              //Serial.println("ajax pozadavek ze stranek");

      myObject["hello"] = String(citac);
  
      jsonString = JSON.stringify(myObject);	
      server_LAN.send(200, F("text/html"), jsonString); });

  server_LAN.serveStatic("/aaa", SD, "/aaa.txt"); // https://randomnerdtutorials.com/esp32-web-server-microsd-card/
  // server.onNotFound(handleNotFound);
  log_i("Koniec funkcie");
}

uint8_t TX_BUF[2500];
void WebServerHandler(u8 s)
{
  char loc_buff[200];
  uint8_t st = w5500.readSnSR(s);
  if (st == SnSR::ESTABLISHED) // 0x17 establiset
  {
    uint16_t size;
    size = w5500.getRXReceivedSize(s);
    if (size > 0)
    {
      // log_i("WebServer handler dostal:%u", size);
      memset(TX_BUF, 0, 2500);
      recv(s, TX_BUF, size);
      // Serial.print(F("\r\nA to : "));
      // Serial.println((char *)TX_BUF);
      //  disconnect(7);
      if (!strncmp((char *)TX_BUF, "GET /hlavne", 11) || !strncmp((char *)TX_BUF, "get /hlavne", 11))
      {
        log_i("Super stranky zadaju HLAVNE");
        zobraz_stranky(s, page_hlavna);
      }
      else if (!strncmp((char *)TX_BUF, "GET /main", 9) || !strncmp((char *)TX_BUF, "get /main", 9))
      {
        log_i("Super stranky zadaju MAIN");
        zobraz_stranky(s, DebugLog_html);
      }
      else if (!strncmp((char *)TX_BUF, "GET /status", 11) || !strncmp((char *)TX_BUF, "GET /status", 11))
      {
        log_i("Bwowser ziada /STATUS");

        char www[120];
        const char *ptr = NULL;
        char rrr[] = "xx";
        char ttt[] = "yy";
        ptr = firmware_text;
        snprintf(www, sizeof(www), " %X:%X:%X:%X:%X:%X", LAN_MAC[0], LAN_MAC[1], LAN_MAC[2], LAN_MAC[3], LAN_MAC[4], LAN_MAC[5]);
        snprintf((char *)TX_BUF, sizeof(TX_BUF), Page_HTML_popisSystemu, www, ptr, rrr, ttt, NazovSiete);
        zobraz_stranky(s, (char *)TX_BUF);

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
      else if (!strncmp((char *)TX_BUF, "GET /get?", 9) || !strncmp((char *)TX_BUF, "get /get?", 9))
      {
        // log_i("Super stranky zadaju GEY AJAX s det?");

        myObject["hello"] = String(citac);

        jsonString = JSON.stringify(myObject);
        jsonString.toCharArray((char *)TX_BUF, jsonString.length() + 1);
        zobraz_stranky(s, (const char *)TX_BUF);
      }

      else if (!strncmp((char *)TX_BUF, "GET /meraj20A_1F", 16))
      {
        log_i("Super stranky zadaju Meraj 20A 1F");
        zobraz_stranky(s, page_hlavna);
        // EEPROM.writeByte(EE_rozsah_Prud, rozsah_20A_1F);
        // EEPROM.commit();
        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_20A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_20A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_20A));
        if (flg.TCPsocketConneknuty == true)
        {
          snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO ze stranky zadaju Meraj 20A 1F !!");
          send(TCPsocket, TX_BUF, strlen((char *)TX_BUF));
        }
      }
      else if (!strncmp((char *)TX_BUF, "GET /meraj20A_3F", 16))
      {
        log_i("Super stranky zadaju Meraj 20A 3F");
        zobraz_stranky(s, page_hlavna);
        // EEPROM.writeByte(EE_rozsah_Prud, rozsah_20A_3F);
        // EEPROM.commit();
        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_20A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_20A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_20A));
        if (flg.TCPsocketConneknuty == true)
        {
          snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO ze stranky zadaju Meraj 20A 3F !!");
          send(TCPsocket, TX_BUF, strlen((char *)TX_BUF));
        }
      }
      else if (!strncmp((char *)TX_BUF, "GET /meraj50A_3F", 16))
      {
        log_i("Super stranky zadaju Meraj 50A 3F");
        zobraz_stranky(s, page_hlavna);
        // EEPROM.writeByte(EE_rozsah_Prud, rozsah_50A_3F);
        // EEPROM.commit();
        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_50A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_50A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_50A));
        if (flg.TCPsocketConneknuty == true)
        {
          snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO ze stranky zadaju Meraj 50A 3F !!");
          send(TCPsocket, TX_BUF, strlen((char *)TX_BUF));
        }
      }
      else if (!strncmp((char *)TX_BUF, "GET /meraj100A_3F", 16))
      {
        log_i("Super stranky zadaju Meraj 100A 3F");
        zobraz_stranky(s, page_hlavna); // zobraz_stranky(s, page_rozsah_100A_3F);
        // EEPROM.writeByte(EE_rozsah_Prud, rozsah_100A_3F);
        // EEPROM.commit();
        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_100A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_100A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_100A));
        if (flg.TCPsocketConneknuty == true)
        {
          snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO ze stranky zadaju Meraj 100A 3F !!");
          send(TCPsocket, TX_BUF, strlen((char *)TX_BUF));
        }
      }

      else if (!strncmp((char *)TX_BUF, "GET /posliUI_20A_1F?", 20) || !strncmp((char *)TX_BUF, "get /posliUI_20A_1F?", 20))
      {
        log_i("Super stranky zadaju GEY AJAX s 20A 1F");
        citac += 0.01f;
        AjaxObjekt["U1"] = String(citac); // String(meranie.U1);
        AjaxObjekt["I1"] = String(meranie.I1);
        jsonString = JSON.stringify(AjaxObjekt);
        jsonString.toCharArray((char *)TX_BUF, jsonString.length() + 1);
        zobraz_stranky(s, (const char *)TX_BUF);
      }

      else if (!strncmp((char *)TX_BUF, "GET /posliUI_data", 17) || !strncmp((char *)TX_BUF, "get /posliUI_data", 17))
      {
        // log_i("Super stranky zadaju GET AJAX s posliUI_data");
        AjaxObjekt["U1"] = String(meranie.U1);
        AjaxObjekt["I1"] = String(meranie.I1);
        AjaxObjekt["U2"] = String(meranie.U2);
        AjaxObjekt["I2"] = String(meranie.I2);
        AjaxObjekt["U3"] = String(meranie.U3);
        AjaxObjekt["I3"] = String(meranie.I3);
        jsonString = JSON.stringify(AjaxObjekt);
        jsonString.toCharArray((char *)TX_BUF, jsonString.length() + 1);
        zobraz_stranky(s, (const char *)TX_BUF);
      }
      delay(10);
      disconnect(s);
    }
  }

  else if (st == SnSR::CLOSE_WAIT) // 0x1c  SOCK_CLOSE_WAIT
  {
    disconnect(s);
  }
  else if (st == SnSR::CLOSED) //  0x00 SOCK_CLOSED
  {
    socket(s, 1, 80, 0x00);
  }
  else if (st == SnSR::INIT) //  0x13 SOCK_Init
  {
    listen(s);
  }
}

void TCP_handler(u8 s)
{
  char loc_buff[200];
  uint8_t st = w5500.readSnSR(s);
  if (st == SnSR::ESTABLISHED)
  {
    flg.TCPsocketConneknuty = true;

    uint16_t size;
    size = w5500.getRXReceivedSize(s);
    if (size > 0)
    {
      log_i("TCP handler dostal:%u", size);
      memset(TX_BUF, 0, 2500);
      recv(s, TX_BUF, size);
      // Serial.print(F("\r\nA to : "));
      // Serial.println((char *)TX_BUF);
      //  disconnect(7);
      if (!strncmp((char *)TX_BUF, "GET /hlavne", 11) || !strncmp((char *)TX_BUF, "get /hlavne", 11))
      {
        log_i("Super stranky zadaju HLAVNE");
        // zobraz_stranky(page_hlavne);
      }
      else if (!strncmp((char *)TX_BUF, "GET /main", 9) || !strncmp((char *)TX_BUF, "get /main", 9))
      {
        log_i("Super stranky zadaju MAIN");
        // zobraz_stranky(DebugLog_html);
      }

      // pro MAC:00:04:1F:B7:47:85
      //  Energy_calib$01$07$00$00 $01$70$ff$ff $01$4a$00$00 $ff$01$00$00 $01$00$00$00 $01$00$00$00 $01$00$00$00 $00$01$00$00 $00$00$00$00
      //  Energy_c_20A$01$10$d0$10 $01$10$d3$10 $01$00$d2$10
      //  Energy_c_50A$01$00$c5$10 $01$00$c6$10 $01$00$c6$10
      //  Energy__100A$01$00$bb$10 $01$00$bd$10 $01$00$bd$10

      // pro MAC: 00:E0:4C:18:82:1C  MaC:$00$e0$4c$18$82$1C
      //  Energy_calib$11$b0$04$00 $11$40$04$00 $11$00$05$00 $ff$01$00$00 $01$00$00$00 $01$00$00$00 $e0$ff$ff$ff $e0$ff$ff$ff $e0$ff$ff$ff
      //  Energy_c_20A$01$10$e5$10 $01$10$e3$10 $01$00$e5$10
      //  Energy_c_50A$01$00$d6$10 $01$00$d7$10 $01$90$d8$10
      //  Energy__100A$01$60$cd$10 $01$a0$cd$10 $01$00$d5$10

      // pro MAC: 00:14:78:7C:62:02  MaC:$00$14$78$7C$62$02
      //  Energy_calib$11$bb$02$00$11$93$04$00$11$7b$02$00$ff$01$00$00$01$00$00$00$01$00$00$00$e0$ff$ff$ff$e0$ff$ff$ff$e0$ff$ff$ff
      //  Energy_c_20A$01$90$de$10$01$10$de$10$01$40$dc$10
      //  Energy_c_50A$01$de$d2$10$01$00$d2$10$01$90$d0$10
      //  Energy__100A$01$60$cd$10 $01$a0$cd$10 $01$00$d5$10
      else if (!strncmp((const char *)TX_BUF, "MaC:", 4)) // MaC:$81$20$13$57$C8$CA  MaC:$00$e0$4c$18$82$1C  MaC:$00$14$78$7C$62$02
      {
        for (u8 i = 0; i < 6; i++)
        {
          LAN_MAC[i] = TX_BUF[i + 4];
        }

        EEPROM.writeBytes(EE_MAC_LAN, LAN_MAC, 6);
        EEPROM.commit();

        snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO nastavenie MAC adrese");
        send(s, TX_BUF, strlen((char *)TX_BUF));
      }

      else if (!strncmp((const char *)TX_BUF, "Energy_calib", 12)) // tu sa kalibruje napetie a offsety AJ PRUDU!!
      {
        EEPROM.writeLong(EE_Vin_gain_1, *(i32 *)&TX_BUF[12]);
        EEPROM.writeLong(EE_Vin_gain_2, *(i32 *)&TX_BUF[16]);
        EEPROM.writeLong(EE_Vin_gain_3, *(i32 *)&TX_BUF[20]);
        EEPROM.writeLong(EE_Vin_offset_1, *(i32 *)&TX_BUF[24]);
        EEPROM.writeLong(EE_Vin_offset_2, *(i32 *)&TX_BUF[28]);
        EEPROM.writeLong(EE_Vin_offset_3, *(i32 *)&TX_BUF[32]);
        EEPROM.writeLong(EE_Iin_offset_1, *(i32 *)&TX_BUF[36]);
        EEPROM.writeLong(EE_Iin_offset_2, *(i32 *)&TX_BUF[40]);
        EEPROM.writeLong(EE_Iin_offset_3, *(i32 *)&TX_BUF[44]);

        EEPROM.commit();

        ADE9078_Wr32(ADDR_AVGAIN, EEPROM.readLong(EE_Vin_gain_1));
        ADE9078_Wr32(ADDR_BVGAIN, EEPROM.readLong(EE_Vin_gain_2));
        ADE9078_Wr32(ADDR_CVGAIN, EEPROM.readLong(EE_Vin_gain_3));
        ADE9078_Wr32(ADDR_AVRMSOS, EEPROM.readLong(EE_Vin_offset_1));
        ADE9078_Wr32(ADDR_BVRMSOS, EEPROM.readLong(EE_Vin_offset_2));
        ADE9078_Wr32(ADDR_CVRMSOS, EEPROM.readLong(EE_Vin_offset_3));
        ADE9078_Wr32(ADDR_AIRMSOS, EEPROM.readLong(EE_Iin_offset_1));
        ADE9078_Wr32(ADDR_BIRMSOS, EEPROM.readLong(EE_Iin_offset_2));
        ADE9078_Wr32(ADDR_CIRMSOS, EEPROM.readLong(EE_Iin_offset_3));

        snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO Energy_calib na napetie !!");
        send(s, TX_BUF, strlen((char *)TX_BUF));
      }
      else if (!strncmp((const char *)TX_BUF, "Energy_c_20A", 12))
      {
        EEPROM.writeLong(EE_Iin_gain_1_20A, *(i32 *)&TX_BUF[12]);
        EEPROM.writeLong(EE_Iin_gain_2_20A, *(i32 *)&TX_BUF[16]);
        EEPROM.writeLong(EE_Iin_gain_3_20A, *(i32 *)&TX_BUF[20]);

        EEPROM.commit();

        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_20A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_20A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_20A));

        snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO Energy_calib na 20A !!");
        send(s, TX_BUF, strlen((char *)TX_BUF));
      }
      else if (!strncmp((const char *)TX_BUF, "Energy_c_50A", 12))
      {
        EEPROM.writeLong(EE_Iin_gain_1_50A, *(i32 *)&TX_BUF[12]);
        EEPROM.writeLong(EE_Iin_gain_2_50A, *(i32 *)&TX_BUF[16]);
        EEPROM.writeLong(EE_Iin_gain_3_50A, *(i32 *)&TX_BUF[20]);

        EEPROM.commit();

        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_50A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_50A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_50A));

        snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO Energy_calib na 50A!!");
        send(s, TX_BUF, strlen((char *)TX_BUF));
      }
      else if (!strncmp((const char *)TX_BUF, "Energy__100A", 12))
      {
        EEPROM.writeLong(EE_Iin_gain_1_100A, *(i32 *)&TX_BUF[12]);
        EEPROM.writeLong(EE_Iin_gain_2_100A, *(i32 *)&TX_BUF[16]);
        EEPROM.writeLong(EE_Iin_gain_3_100A, *(i32 *)&TX_BUF[20]);

        EEPROM.commit();

        ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_100A));
        ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_100A));
        ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_100A));

        snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\n*****DOSLO Energy_calib na 100A!!");
        send(s, TX_BUF, strlen((char *)TX_BUF));
      }
    }
  }

  else if (st == SnSR::CLOSE_WAIT)
  {
    disconnect(s);
    log_i("%d:CloseWait\r\n", s);
  }
  else if (st == SnSR::CLOSED)
  {
    socket(s, 1, 10001, 0x00);
    log_i("%d:Closed\r\n", s);
  }
  else if (st == SnSR::INIT) // SOCK_Init
  {
    listen(s);
    log_i("%d:Listen, TCPsocket\r\n", s);
    flg.TCPsocketConneknuty = false;
  }
}

void Task_handle_ADE9078_Code(void *arg)
{
  log_i("Spustam Task ADE9078");
  ADE9078_init();
  log_i("Nacitane Signature ADE9078 ma byt podla PDF 0x40, ale aj v AWTools vraca 0x80: %X", ADE9078_GetVersion());
  log_i("Nacitane Register PSM2_CFG ma byt 0x1F:: %X", ADE9078_Rd_u16(ADDR_PSM2_CFG));
  log_i("Nacitane Register CONFIG 5 ma byt 0x63: %X", ADE9078_Rd_u16(ADDR_CONFIG5));

  meranie.pocet_samplu = 1; // pozor na deleni nulu!!!
  meranie.U1avg = meranie.U1avg = 0;
  meranie.U2avg = meranie.U2avg = 0;
  meranie.U3avg = meranie.U3avg = 0;
  meranie.I1avg = meranie.I1avg = 0;
  meranie.I2avg = meranie.I2avg = 0;
  meranie.I3avg = meranie.I3avg = 0;
  uint16_t verzeADEcipu;
  verzeADEcipu = ADE9078_Rd_u16(ADDR_RUN);
  ADE9078_Wr16(ADDR_RUN, 1);
  delay(100);
  verzeADEcipu = ADE9078_Rd_u16(ADDR_RUN);
  delay(100);

  log_i("Nacitane i32 z adrese EE_Vin_gain_1 :%li", EEPROM.readLong(EE_Vin_gain_1));
  ADE9078_Wr32(ADDR_AVGAIN, EEPROM.readLong(EE_Vin_gain_1));
  ADE9078_Wr32(ADDR_BVGAIN, EEPROM.readLong(EE_Vin_gain_2));
  ADE9078_Wr32(ADDR_CVGAIN, EEPROM.readLong(EE_Vin_gain_3));
  ADE9078_Wr32(ADDR_AVRMSOS, EEPROM.readLong(EE_Vin_offset_1));
  ADE9078_Wr32(ADDR_BVRMSOS, EEPROM.readLong(EE_Vin_offset_2));
  ADE9078_Wr32(ADDR_CVRMSOS, EEPROM.readLong(EE_Vin_offset_3));

  ADE9078_Wr32(ADDR_AIGAIN, EEPROM.readLong(EE_Iin_gain_1_20A));
  ADE9078_Wr32(ADDR_BIGAIN, EEPROM.readLong(EE_Iin_gain_2_20A));
  ADE9078_Wr32(ADDR_CIGAIN, EEPROM.readLong(EE_Iin_gain_3_20A));
  ADE9078_Wr32(ADDR_AIRMSOS, 0); // EEPROM.readLong(EE_Iin_offset_1));
  ADE9078_Wr32(ADDR_BIRMSOS, EEPROM.readLong(EE_Iin_offset_2));
  ADE9078_Wr32(ADDR_CIRMSOS, EEPROM.readLong(EE_Iin_offset_3));

  while (1)
  {
    meranie.U1 = ADE9078_Rd_u32(ADDR_AVRMS);
    meranie.U1 /= DelPomer_U;
    // log_i("Nacitane meranie.U1: %4.2f", meranie.U1);
    // log_i("Nacitane Reg32 a to ADDR_AVRMS je: %lu", ADE9078_Rd_u32(ADDR_AVRMS));

    if (meranie.U1 > 50)
    {

      meranie.U2 = ADE9078_Rd_u32(ADDR_BVRMS);
      meranie.U2 /= DelPomer_U;
      meranie.U3 = ADE9078_Rd_u32(ADDR_CVRMS);
      meranie.U3 /= DelPomer_U;

      meranie.I1 = ADE9078_Rd_u32(ADDR_AIRMS);
      meranie.I1 /= DelPomer_I;
      meranie.I2 = ADE9078_Rd_u32(ADDR_BIRMS);
      meranie.I2 /= DelPomer_I;
      meranie.I3 = ADE9078_Rd_u32(ADDR_CIRMS);
      meranie.I3 /= DelPomer_I;

      meranie.Ang_a = (float)ADE9078_Rd_u16(ADDR_ANGL_VA_IA);
      meranie.Ang_a *= 0.03515625f;
      meranie.Ang_b = (float)ADE9078_Rd_u16(ADDR_ANGL_VB_IB);
      meranie.Ang_b *= 0.03515625f;
      meranie.Ang_c = (float)ADE9078_Rd_u16(ADDR_ANGL_VC_IC);
      meranie.Ang_c *= 0.03515625f;
      meranie.Freq = ADE9078_Rd_u32(ADDR_APERIOD);
      meranie.Freq /= 104857.24f;
    }
    else
    {
      meranie.U1 = meranie.U2 = meranie.U3 = meranie.I1 = meranie.I2 = meranie.I3 = 0;
      meranie.Ang_a = meranie.Ang_b = meranie.Ang_c = 0;

      meranie.vykonP1 = meranie.vykonP2 = meranie.vykonP3 = 0;
      meranie.vykonQ1 = meranie.vykonQ2 = meranie.vykonQ3 = 0;
      meranie.vykonS1 = meranie.vykonS2 = meranie.vykonS3 = 0;
      meranie.Freq = 0;
    }

    delay(1000);
  }
}

void t1_MAIN(void *arg)
{
  log_i("Spustam Task1");

  // FuncServer_On();

  while (1)
  {
    UDPhandler();
    // server.handleClient();

    WebServerHandler(6);

    TCP_handler(TCPsocket);
    // log_i("Task 1 loop");
    delay(10);
  }
}

// static EthernetWebServer serverr(8080);
void t2_ethTask(void *arg)
{

  log_i("Spustam Task2");

  while (1)
  {
    // ESP.getFreeHeap();  xPortGetFreeHeapSize()
    if (digitalRead(LEDstatus_pin) == 1)
    {
      digitalWrite(LEDstatus_pin, 0);
    }
    else
    {
      digitalWrite(LEDstatus_pin, 1);
    }

    log_i("RTOS free HeAP:%d", ESP.getFreeHeap());
    if (flg.TCPsocketConneknuty == true)
    {
      snprintf((char *)TX_BUF, sizeof(TX_BUF), "\r\nADE9078 Data: U1 = %.03fV  U2 = %.03fV   U2 = %.03fV  I1 = %.03fA  I2 = %.03fA  I3 = %.03fA",
               meranie.U1, meranie.U2, meranie.U3, meranie.I1, meranie.I2, meranie.I3);
      send(TCPsocket, TX_BUF, strlen((char *)TX_BUF));
    }
    delay(1000);
  }
}

void zobraz_stranky(u8 socket, const char *ptrNaStranky)
{
  char locBuff[2048]; 
  uint32_t index = 0;

  // log_i("Stranky maju delku %u", strlen(ptrNaStranky)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));

  snprintf(locBuff, sizeof(locBuff), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %u\r\nConnection: close\r\n\r\n", strlen((char *)ptrNaStranky));
  send(socket, (u8 *)locBuff, strlen((char *)locBuff));

  memset(locBuff, 0, sizeof(locBuff));
  u32 kolkoPoslnaych = 0;
  u32 velkostStranek = strlen(ptrNaStranky);
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], velkostStranek);
      send(socket, (u8 *)locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], zostava);
        send(socket, (u8 *)locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], 1000);
        send(socket, (u8 *)locBuff, 1000);
        kolkoPoslnaych += 1000;
      }
    }
  } while (kolkoPoslnaych < velkostStranek);
}
