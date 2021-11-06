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
//#include <TaskScheduler.h>

bool GetData_flag = false; 
// Task t1(1050, TASK_FOREVER, &t1_MAIN);
// Task t2(20, TASK_FOREVER, &t2_ethTask);
// Scheduler runner;
TaskHandle_t htask1;
TaskHandle_t htask2;

const char *www_username = "qqq";
const char *www_password = "www";

IPAddress local_IP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

EthernetWebServer server(80);

Ticker timer_10ms(Loop_10ms, 100, 0, MILLIS);
JSONVar myObject;

uint16_t citac = 0;
; // int reqCount = 0; // number of requests received

void Loop_10ms()
{
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

  Serial.println(F("Desel pozadavek na main"));
  Serial.println("Main ma delku" + String(strlen(DebugLog_html)));
  server.setContentLength(strlen(DebugLog_html));

  // const String *ptrString = &locBuff;
  memcpy(locBuff, DebugLog_html, 100);
  server.send(200, "text/html", locBuff, 0);
  uint16_t kolkoPoslnaych = 0;
  do
  {
    memcpy(locBuff, &DebugLog_html[kolkoPoslnaych], 500);
    server.sendContent(locBuff, 500);
    kolkoPoslnaych += 500;
  } while (kolkoPoslnaych < strlen(DebugLog_html));

  // File file = SD.open("/aaa.txt", FILE_READ);
  //  server.streamFile(file, "text/html");
  //  file.close();
}

void hlavne()
{
  char locBuff[2048];
  uint32_t index = 0;

  Serial.println(F("Desel pozadavek na hlavne"));
  Serial.println("Hlavne ma delku" + String(strlen(page_hlavne)));
  server.setContentLength(strlen(page_hlavne));

  memcpy(locBuff, page_hlavne, 100);
  server.send(200, "text/html", locBuff, 0);
  uint16_t kolkoPoslnaych = 0;
  do
  {
    memcpy(locBuff, &page_hlavne[kolkoPoslnaych], 1000);
    server.sendContent(locBuff, 1000);
    kolkoPoslnaych += 1000;
  } while (kolkoPoslnaych < strlen(page_hlavne));

  // File file = SD.open("/aaa.txt", FILE_READ);
  //  server.streamFile(file, "text/html");
  //  file.close();
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
  Serial.println("Spustam applikaciu...XXXX1234");
  System_init();
  NacitajSuborzSD();

  FuncServer_On();

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
  NacitajSuborzSD();
  
  // assert(rc == pdPASS);
}

void loop(void)
{
  esp_task_wdt_reset();
  // runner.execute();
  timer_10ms.update();
  server.handleClient();
  TCPhandler();
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

  // server.serveStatic("/page2", SPIFFS, "/page2.html");
  // server.serveStatic("/page3", SPIFFS, "/page3.html");
  server.serveStatic("/vlajka", SPIFFS, "/CanadaFlag_3.jpg");

  server.on("/main", HTTP_GET, testFunct);
  server.on("/hlavne", HTTP_GET, hlavne);

  server.on("/subor", HTTP_GET, ReadSuborzSD);

  server.on("/get", HTTP_GET, []()
            { 
              Serial.println("ajax pozadavek ze stranek");

      myObject["hello"] = String(citac);
  
        String jsonString = JSON.stringify(myObject);	
              server.send(200, F("text/html"), jsonString); });

  server.serveStatic("/aaa", SD, "/aaa.txt"); // https://randomnerdtutorials.com/esp32-web-server-microsd-card/
  //server.onNotFound(handleNotFound);
  log_i("Koniec funkcie");
}

uint8_t TX_BUF[2500];
void TCPhandler(void)
{
  uint8_t st = w5500.readSnSR(6);
  if (st == 0x17) // establiset
  {
    uint16_t size;
    size = w5500.getRXReceivedSize(6);
    if (size > 0)
    {
      ET_LOGWARN1(F("Tcp 1 handler dostal:"), size);
      memset(TX_BUF, 0, 2500);
      recv(6, TX_BUF, size);
      Serial.print(F("\r\nA to : "));
      Serial.println((char *)TX_BUF);
      // disconnect(7);
    }
  }

  else if (st == 0x1c) // SOCK_CLOSE_WAIT
  {
    disconnect(6);
  }
  else if (st == 0x00) // SOCK_CLOSED
  {
    socket(6, 1, 10001, 0x00);
  }
  else if (st == 0x13) // SOCK_Init
  {
    listen(6);
  }
}