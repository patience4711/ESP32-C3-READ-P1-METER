
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <AsyncEventSource.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <DNSServer.h>
#include "driver/uart.h"
//#include "esp_system.h"
#include "OTA.h"
#include <Update.h>
//#include <Hash.h>
//#include "PSACrypto.h"

#include <ESP32wifiConnect.h>
ESP32wifiConnect my_wifiConnect;

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h"           // ha for brownout
#include "soc/rtc_cntl_reg.h"  // ha for brownout
#include <esp_task_wdt.h>      // ha
#include <rtc_wdt.h>
   
#include <SPIFFS.h>
#include "FS.h"

AsyncWebServer server(80); //
AsyncEventSource events("/events"); 
AsyncWebSocket ws("/ws");

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const byte DNS_PORT = 53;
DNSServer dnsServer;

#include <HTTPClient.h>
WiFiClient espClient ;
HTTPClient http;
PubSubClient MQTT_Client(espClient) ;


#include "CRC16.h"

#include "HTML.h"
#include "AAA_MENUPAGE.h"
#include "AAA_HOMEPAGE.h"

#include "REPORT.h"

  //char ssid[33] = ""; // was 33 
  //char pass[64] = ""; // was 40
  //bool tryConnectFlag = false;

 // variables wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;


  char requestUrl[20] = {"/"}; // to remember from which webpage we came  

// variables mqtt ********************************************
  char  Mqtt_Broker[30]=    {"192.168.0.100"};
  char  Mqtt_outTopic[26] = {"domoticz/in"}; // was 26
  char  Mqtt_Username[26] = {""};
  char  Mqtt_Password[26] = {""};
  char  Mqtt_Clientid[26] = {""};
  int   Mqtt_Port =  1883;
  int   Mqtt_Format = 0;

// variables domoticz ********************************************
  char dom_Address[30]={"192.168.0.100"};
  int  dom_Port =  8080;
  int  gas_Idx = 123;
  int  el_Idx = 456;

  bool baud9600;
  bool rxInvert;
  bool ledSignal;
  
  int event = 0;
  
  int dst;
  bool timeRetrieved = false;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  //unsigned long lastP1ReadTime = 0;

#define knop              0  //
//#define led_onb           8  // onboard led was 2
#define P1_ENABLE         5 // gpio5 if high the RX of the meter is pulled high

#define OBIS_SMR             "1-3:0.2.8("
#define OBIS_CON_LT   "1-0:1.8.1("
#define OBIS_CON_HT   "1-0:1.8.2("
#define OBIS_RET_LT   "1-0:2.8.1("
#define OBIS_RET_HT   "1-0:2.8.2(" 

#define OBIS_POWER_C1 "1-0:21.7.0("
#define OBIS_POWER_R1 "1-0:22.7.0("
#define OBIS_POWER_C2 "1-0:41.7.0("
#define OBIS_POWER_R2 "1-0:42.7.0("
#define OBIS_POWER_C3 "1-0:61.7.0("
#define OBIS_POWER_R3 "1-0:62.7.0("

#define OBIS_GAS      "0-1:24.2.1("

struct MeterData {
    uint8_t smr;
    float   con_lt;
    float   con_ht;
    float   ret_lt;
    float   ret_ht;
    uint16_t   pwr_con[3];
    uint16_t   pwr_ret[3];
    float   gas;
};
MeterData meter;

String toSend = "";
 
int value = 0; 
int aantal = 0;


int Mqtt_stateIDX = 0;

// Vars to store meter readings//
//float ECON_LT = 0; //Meter reading Electrics - consumption low tariff
//float ECON_HT = 0; //Meter reading Electrics - consumption high tariff
//float ERET_LT = 0; //Meter reading Electrics - return low tariff
//float ERET_HT = 0; //Meter reading Electrics - return high tariff
//float PACTUAL_CON = 0;  //Meter reading Electrics - Actual consumption
//float PACTUAL_RET = 0;  //Meter reading Electrics - Actual return
//float mGAS = 0;  //Meter reading Gas
//long prevGAS = 0;
//int SMR = 0;
//char meterType[30]={"UNKNOWN"};
// setup a struct for the values of each month
typedef struct{
  float EC_LT = 0;
  float EC_HT = 0;
  float ER_LT = 0;
  float ER_HT = 0;
  float mGAS  = 0;
} m_values; 
m_values MVALS[13]; 

//unsigned int currentCRC = 0; //needs to be global?

bool diagNose = true;
int actionFlag = 0;
char txBuffer[50];
bool USB_serial = true;
char meterType[30];
bool polled = false;
//bool Polling = false;
int pollFreq = 300;
bool threePhase = false;
// voor testing
int www = 0;
int maxTry = 5;
char teleGram[1060]={"not polled"};
char readCRC[5];
int testLength;
int errorCode = 0;
int securityLevel = 0;
//bool tested = false;
//char logChar[150] = {"log: "};
#define P1_MAXLINELENGTH 1050
//t currentCRC=0;
//t tests = 0; // to prevent for an endless loop
//ol endsign = false;
//bool testTelegram = false;
char timeStamp[20]={"not polled"};
// // bluetooth settings and vars
// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

#include <Preferences.h>
Preferences prefs;