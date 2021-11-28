#include <Arduino.h>
#include <esp_task_wdt.h>
#include "define.h"
#include <SPI.h>
#include "Ethernet3.h"
#include <EthernetWebServer.h>
#include <utility\socket.h>
#include "FS.h"
#include "SD.h"
#include <Ticker.h>
#include "index.html"
#include <Arduino_JSON.h>
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
const char *www_password = "www";

IPAddress local_IP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

EthernetWebServer server(8080);

char NazovSiete[30];
char Heslo[30];

Ticker timer_10ms(Loop_10ms, 100, 0, MILLIS);
static JSONVar myObject;
static String jsonString;

uint16_t citac = 0;
; // int reqCount = 0; // number of requests received

void Loop_10ms()
{
  citac++;
  citac++;
}

void handleNotFound()
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += server.uri();
  if (server.uri() == "/data.txt")
  {

    server.setContentLength(CONTENT_LENGTH_UNKNOWN); // https://bleepcoder.com/arduino/226633148/server-send-fails-to-send-strings-html-greater-than-6600
    server.send(200, "text/html", "xx");
    for (uint16_t i = 0; i < 300; i++)
    {
      server.sendContent("123451234512345123451234512345123451234512345123451234512345123451234512345123451234512345123451234512345123456\r\n"); /* code */ /* code */
    }
    server.sendContent("KOnec");
    return;
  }
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, F("text/plain"), message);
}

void testFunct()
{
  char locBuff[2048];
  uint32_t index = 0;

  log_i("Desel pozadavek na main");
  log_i("Main ma delku %u", strlen(DebugLog_html)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));
  server.setContentLength(strlen(DebugLog_html));

  memset(locBuff, 0, sizeof(locBuff));
  server.send(200, "text/html", locBuff);
  u32 kolkoPoslnaych = 0;
  u32 velkostStranek = strlen(DebugLog_html);
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], velkostStranek);
      server.sendContent(locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], zostava);
        server.sendContent(locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], 1000);
        server.sendContent(locBuff, 1000);
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
  log_i("Main ma delku %u", strlen(page_hlavne)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));
  server.setContentLength(strlen(page_hlavne));

  memset(locBuff, 0, sizeof(locBuff));
  server.send(200, "text/html", locBuff);
  u32 kolkoPoslnaych = 0;
  u32 velkostStranek = strlen(page_hlavne);
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &page_hlavne[kolkoPoslnaych], velkostStranek);
      server.sendContent(locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &page_hlavne[kolkoPoslnaych], zostava);
        server.sendContent(locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &page_hlavne[kolkoPoslnaych], 1000);
        server.sendContent(locBuff, 1000);
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
  server.send(200, F("text/plain"), F("subora nacitany"));

  NacitajSuborzSD();
}

char buff_extra[4001];

void setup(void)
{

  Serial.begin(115200);
  Serial.println("Spustam applikaciu...Y 11");
  System_init();
  // NacitajSuborzSD();

  NacitajEEPROM_setting();
  // FuncServer_On();

  server.begin();

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

  log_i("Idem citat subore z SD karty po init web");
  // NacitajSuborzSD();

  // assert(rc == pdPASS);

  xTaskCreatePinnedToCore(
      t1_MAIN, // Task function
      "task1", // Name
      13000,   // Stack size
      nullptr, // Parameters
      1,       // Priority
      &htask1, // handle
      0        // CPU
  );

  xTaskCreatePinnedToCore(
      t2_ethTask, // Task function
      "task2",    // Name
      13000,      // Stack size
      nullptr,    // Parameters
      1,          // Priority
      &htask2,    // handle
      0           // CPU
  );

  xTaskCreatePinnedToCore(
      Task_handle_ADE9078_Code, // Task function
      "Task_ADE9078",           // Name
      13000,                    // Stack size
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
  // server.handleClient();

  delay(5);
}

void FuncServer_On(void)
{
  log_i("Zaciatok funkcie");
  server.on("/", HTTP_GET, []()
            {
              ET_LOGWARN(F("Dosel pozadavek na Handle ROot"));
              ET_LOGWARN1(F("TCP 0 status reg:"), w5500.readSnSR(7));
              ET_LOGWARN1(F("TCP1 status reg:"), w5500.readSnSR(6));

              server.send(200, F("text/plain"), F("Deska jede")); });

  server.on(F("/heslo"), []()
            {
              if (!server.authenticate(www_username, www_password))
              {
                return server.requestAuthentication();
              }
              server.send(200, F("text/plain"), F("Login OK \r\ntoto by ale asi mohel byt kuuuuuurna dlhy text")); });

  // server.on("/list", HTTP_GET, printDirectory);

  server.serveStatic("/page2", SPIFFS, "/page2.html");
  server.serveStatic("/page3", SPIFFS, "/page3.html");
  server.serveStatic("/vlajka1", SPIFFS, "/CanadaFlag_1.png");
  server.serveStatic("/vlajka2", SPIFFS, "/CanadaFlag_2.png");
  server.serveStatic("/vlajka3", SPIFFS, "/CanadaFlag_3.jpg");
  server.on("/main", HTTP_GET, testFunct);
  server.on("/hlavne", HTTP_GET, hlavne);

  server.on("/subor", HTTP_GET, ReadSuborzSD);

  server.on("/get", HTTP_GET, []()
            { 
              //Serial.println("ajax pozadavek ze stranek");

      myObject["hello"] = String(citac);
  
      jsonString = JSON.stringify(myObject);	
      server.send(200, F("text/html"), jsonString); });

  server.serveStatic("/aaa", SD, "/aaa.txt"); // https://randomnerdtutorials.com/esp32-web-server-microsd-card/
  // server.onNotFound(handleNotFound);
  log_i("Koniec funkcie");
}

uint8_t TX_BUF[2500];
void WebServerHandler(void)
{
  char loc_buff[200];
  uint8_t st = w5500.readSnSR(6);
  if (st == 0x17) // establiset
  {
    uint16_t size;
    size = w5500.getRXReceivedSize(6);
    if (size > 0)
    {
      // log_i("WebServer handler dostal:%u", size);
      memset(TX_BUF, 0, 2500);
      recv(6, TX_BUF, size);
      // Serial.print(F("\r\nA to : "));
      // Serial.println((char *)TX_BUF);
      //  disconnect(7);
      if (!strncmp((char *)TX_BUF, "GET /hlavne", 11) || !strncmp((char *)TX_BUF, "get /hlavne", 11))
      {
        log_i("Super stranky zadaju HLAVNE");
        zobraz_stranky(page_hlavne);
      }
      else if (!strncmp((char *)TX_BUF, "GET /main", 9) || !strncmp((char *)TX_BUF, "get /main", 9))
      {
        log_i("Super stranky zadaju MAIN");
        zobraz_stranky(DebugLog_html);
      }
      else if (!strncmp((char *)TX_BUF, "GET /get?", 9) || !strncmp((char *)TX_BUF, "get /get?", 9))
      {
        // log_i("Super stranky zadaju GEY AJAX s det?");

        myObject["hello"] = String(citac);

        jsonString = JSON.stringify(myObject);
        jsonString.toCharArray((char *)TX_BUF, jsonString.length() + 1);
        zobraz_stranky((const char *)TX_BUF);
      }

      delay(100);
      disconnect(6);
    }
  }

  else if (st == 0x1c) // SOCK_CLOSE_WAIT
  {
    disconnect(6);
  }
  else if (st == 0x00) // SOCK_CLOSED
  {
    socket(6, 1, 80, 0x00);
  }
  else if (st == 0x13) // SOCK_Init
  {
    listen(6);
  }
}

void TCP_handler(void)
{
  char loc_buff[200];
  uint8_t st = w5500.readSnSR(7);
  if (st == 0x17) // establiset
  {
    uint16_t size;
    size = w5500.getRXReceivedSize(7);
    if (size > 0)
    {
      log_i("TCP handler dostal:%u", size);
      memset(TX_BUF, 0, 2500);
      recv(7, TX_BUF, size);
      // Serial.print(F("\r\nA to : "));
      // Serial.println((char *)TX_BUF);
      //  disconnect(7);
      if (!strncmp((char *)TX_BUF, "GET /hlavne", 11) || !strncmp((char *)TX_BUF, "get /hlavne", 11))
      {
        log_i("Super stranky zadaju HLAVNE");
        //zobraz_stranky(page_hlavne);
      }
      else if (!strncmp((char *)TX_BUF, "GET /main", 9) || !strncmp((char *)TX_BUF, "get /main", 9))
      {
        log_i("Super stranky zadaju MAIN");
        //zobraz_stranky(DebugLog_html);
      }
    }
  }

  else if (st == 0x1c) // SOCK_CLOSE_WAIT
  {
    disconnect(7);
  }
  else if (st == 0x00) // SOCK_CLOSED
  {
    socket(7, 1, 10001, 0x00);
  }
  else if (st == 0x13) // SOCK_Init
  {
    listen(7);
  }
}

void Task_handle_ADE9078_Code(void *arg)
{

  log_i("Spustam Tas ADE90781");
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

  while (1)
  {
    meranie.U1 = ADE9078_Rd_u32(ADDR_AVRMS);
    meranie.U1 /= DelPomer_U;
    // log_i("Nacitane meranie.U1: %4.2f", meranie.U1);
    // log_i("Nacitane Reg32 a to ADDR_AVRMS je: %lu", ADE9078_Rd_u32(ADDR_AVRMS));

    delay(1000);
  }
}

void t1_MAIN(void *arg)
{
  log_i("Spustam Task1");

  FuncServer_On();

  while (1)
  {
    UDPhandler();
    server.handleClient();
    WebServerHandler();
    TCP_handler();
    // log_i("Task 1 loop");
    delay(10);
  }
}

static EthernetWebServer serverr(8080);
void t2_ethTask(void *arg)
{

  log_i("Spustam Task2");

  while (1)
  {
    // ESP.getFreeHeap()
    log_i("RTOS free HeAP:%d", xPortGetFreeHeapSize());
    delay(5000);
  }
}

void zobraz_stranky(const char *ptrNaStranky)
{
  char locBuff[2048];
  uint32_t index = 0;

  // log_i("Stranky maju delku %u", strlen(ptrNaStranky)); // Serial.println("Main ma delku" + String(strlen(DebugLog_html)));

  sprintf(locBuff, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %u\r\nConnection: close\r\n\r\n", strlen((char *)ptrNaStranky));
  send(6, (u8 *)locBuff, strlen((char *)locBuff));

  memset(locBuff, 0, sizeof(locBuff));
  u32 kolkoPoslnaych = 0;
  u32 velkostStranek = strlen(ptrNaStranky);
  do
  {
    if (velkostStranek < 1000)
    {
      memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], velkostStranek);
      send(6, (u8 *)locBuff, velkostStranek);
      kolkoPoslnaych = velkostStranek;
    }
    else
    {
      u32 zostava = velkostStranek - kolkoPoslnaych;
      if (zostava < 1000)
      {
        memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], zostava);
        send(6, (u8 *)locBuff, zostava);
        kolkoPoslnaych += zostava;
      }
      else
      {
        memcpy(locBuff, &ptrNaStranky[kolkoPoslnaych], 1000);
        send(6, (u8 *)locBuff, 1000);
        kolkoPoslnaych += 1000;
      }
    }
  } while (kolkoPoslnaych < velkostStranek);
}
