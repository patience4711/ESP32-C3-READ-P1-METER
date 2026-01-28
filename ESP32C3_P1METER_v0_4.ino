/* http://domoticx.com/arduino-p1-poort-telegrammen-uitlezen/
 * http://domoticx.com/p1-poort-slimme-meter-hardware/
 * https://github.com/romix123/P1-wifi-gateway/blob/main/src/P1WG2022current.ino
 * http://www.gejanssen.com/howto/Slimme-meter-uitlezen/#mozTocId935754
 * 
 * 
 in this version we first read the whole telegram in an array and try to verify the crc
 platform: ESP32 c3 dev
 partition scheme: minimal spiffs with ota
 board version 3.0.7 ?? other versions won't compile


*/

#include <ArduinoJson.h>

#include <ESPAsyncWebSrv.h>
//#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncEventSource.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <DNSServer.h>

#include "OTA.h"
#include <Update.h>
//#include <Hash.h>
#include "PSACrypto.h"


#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h"           // ha for brownout
#include "soc/rtc_cntl_reg.h"  // ha for brownout
#include <esp_task_wdt.h>      // ha
#include <rtc_wdt.h>

#include "SPIFFS.h"
#include "FS.h"

//#include "AsyncJson.h"
#include <Arduino.h>

#include <Preferences.h>  //#include <AsyncTCP.h>
Preferences prefs;
//
//#include "Async_TCP.h" //we include the customized one
#include "driver/uart.h"

//#include "esp_heap_caps.h"
/* this was compiled with board version
this software switches  small amount of white leds
by putting some gpio's in parallel
to source more current.
 */ 
#include <esp_wifi.h>   //Used for mpdu_rx_disable android workaround 
#include <WiFi.h>
#include <DNSServer.h> 

//#include "OTA.H"
#include <Update.h>
//#include <Hash.h>

#define VERSION  "ESP32C3-P1METER"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

//#include "soc/soc.h" // ha for brownout
//#include "soc/rtc_cntl_reg.h" // ha for brownout
//#include <esp_task_wdt.h> // ha
//#include "soc/rtc_wdt.h"
           
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
//#include "AsyncJson.h"
#include <Arduino.h>

//#include <AsyncTCP.h>
//#include "Async_TCP.h" //we include the customized one

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

#include "CRC16.h"

#include "HTML.H"
#include "AAA_MENUPAGE.H"
#include "AAA_HOMEPAGE.H"
#include "PORTAL_HTML.H"
#include "REPORT.H"

  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;

 // variables wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool DTS = true;


  char requestUrl[15] = {""}; // to remember from which webpage we came  

// variables mqtt ********************************************
  char  Mqtt_Broker[30];
  char  Mqtt_outTopic[26]; // was 26
  char  Mqtt_inTopic[26];  // was 26
  char  Mqtt_Username[26];
  char  Mqtt_Password[26];
  char  Mqtt_Clientid[26];
  char  Mqtt_Port[5];
  int   Mqtt_Format = 0;

// variables domoticz ********************************************
  //char dom_Address[30]={"192.168.0.100"};
  //int  dom_Port =  8080;
  int  gas_Idx = 123;
  int  el_Idx = 456;

  int event = 0;
  
  uint8_t dst;
  bool timeRetrieved = false;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  
#define LED_AAN    LOW   //sinc
#define LED_UIT    HIGH
#define knop          0  //
#define led_onb       8  // onboard led was 2
#define P1_ENABLE     5  // gpio5 if high the RX of the meter is pulled high
#define RXP1          3
#define TXP1          2  // not used

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



WiFiClient espClient ;
PubSubClient MQTT_Client(espClient) ;

int Mqtt_stateIDX = 0;

// Vars to store meter readings
//float CON_LT = 0; // - consumption low tariff
//float CON_HT = 0; // - consumption high tariff
//float RET_LT = 0; // - return low tariff
//float RET_HT = 0; // - return high tariff
//float POWER_CON[3] = {0 ,0, 0};  //Actual powe consumption
//float POWER_RET[3] = {0, 0, 0};  //Actual power return
//float mGAS = 0;  //Meter reading Gas
//long prevGAS = 0;
//int SMR = 0;



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

uint8_t diagNose = 0;
int actionFlag = 0;
char txBuffer[50];
//bool USB_serial = true;
bool bootTest;
uint8_t meterType = 0;
bool baudRate9600;
bool rxInvert;
bool polled = false;
int pollFreq = 300;
// voor testing
int www = 0;

char teleGram[1060]={"not polled"};
char readCRC[5];
bool threePhase = false;
int testLength;
int errorCode = 0;
int securityLevel = 0;
//bool tested = false;
//char logChar[150] = {"log: "};
#define P1_MAXLINELENGTH 1050
//t currentCRC=0;
//t tests = 0; // to prevent for an endless loop
//ol endsign = false;
bool testTelegram = false;
char timeStamp[12]={"not polled"};
// bluetooth settings and vars
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//bool blueTooth = false;
//BluetoothSerial SerialBT;
//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
//const char *pin = "9999"; // Change this to more secure PIN.
uint8_t procesId = 1;

// *****************************************************************************
// *                              SETUP
// *****************************************************************************
//#include <SoftwareSerial.h>
// EspSoftwareSerial::UART p1Serial;
 // Define the RX and TX pins for Serial 2
//HardwareSerial p1Serial(1); // doesn't work
//#define RXP1 9
//#define TXP1 10

//#define GPS_BAUD 115200

void setup() {


  

  delay(100);
  Serial.begin(115200);
  Serial.println("The device started");
  Serial.println("Serial1 started at 115200 baud rate");
  
  pinMode(knop, INPUT_PULLUP); // the button gpio4 D2 
  pinMode(led_onb, OUTPUT); // onboard led gpio0 D3

  pinMode(P1_ENABLE, OUTPUT);// with this pin gpio14 (D5)  
  digitalWrite(P1_ENABLE, LOW);
  ledblink(1, 800);
  
  attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);

  SPIFFS_read();
  delay(100);
  // ***********************************************************
  // *             initialize serial 1                         *
  // ***********************************************************
    Serial1.setRxBufferSize(1024);
  if (baudRate9600)
  {
     Serial1.begin(9600, SERIAL_7E1, 3, 2);
  } else {
     Serial1.begin(115200, SERIAL_8N1, 3, 2);
  } 
  if(rxInvert) uart_set_line_inverse(UART_NUM_1, UART_SIGNAL_RXD_INV);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //otherwise ,powered by the meter causes brown-out

// takes care for the return to the last webpage after reboot
 // if (requestUrl[0] != '/' ) strcpy(requestUrl, "/");  // vangnet
 // Serial.println("\n\nrequestUrl = " + String(requestUrl));
  ledblink(3,30); 
 
  // poll the meter at once so that we have a debugfile when there is no wifi
  // dont bother to start wifi first
  if(bootTest) 
  {
    pollFreq = 0; // disable autopoll
    meterPoll(); //we do the first poll to test
  }
  start_wifi(); // start wifi and server

  getTijd(); // retrieve time from the timeserver

// reed monthly values files
  for (int x=1; x < 13; x++) 
  {
  String bestand = "/mvalues_" + String(x) + ".str";  
       readStruct(bestand, x);
//      if(!readStruct(bestand, x)) {
//      Serial.println("populate struct with null");  
//      MVALS[x].EC_LT = 000000.00;
//      MVALS[x].EC_HT = 000000.00;
//      MVALS[x].ER_LT = 000000.00;
//      MVALS[x].ER_HT = 000000.00;
//      MVALS[x].mGAS =  00000.00;       
//  }
}

//  // ****************** mqtt init *********************
       MQTT_Client.setKeepAlive(150);
       MQTT_Client.setBufferSize(512);
       MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port) );
       MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 

  initWebSocket();
  // wether the rx should be revered depends on the meter. So far meterType 1 should have this
  
  
  Serial.println(F("booted up"));
  Serial.println(WiFi.localIP());

  delay(1000);
  ledblink(3,500);

  eventSend(0);
  if ( !timeRetrieved )   getTijd();

 
} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {


//if(moetZenden) send_testGram();
//              polling every 60 seconds 
// ******************************************************************

  unsigned long nu = millis();  // the time the program is running
// we only poll when the freqency != null
if(pollFreq != 0) 
{
   if (nu - laatsteMeting >= 1000UL * pollFreq) 
   {
     if(diagNose) 
        {
          String toLog = String(pollFreq) + " seconds passed";
          consoleOut(toLog);
        }
        laatsteMeting += 1000UL * pollFreq ; // 
        // the p1 meter only transmits when the rx line = high
        digitalWrite(P1_ENABLE, HIGH);
        meterPoll(); 
        digitalWrite(P1_ENABLE, LOW);
   }
}

  if (day() != datum) // if date overflew 
  { 
       if(day() < datum) {
        // its the 1st of the new month 
        // the struct goes from 0 - 11
        writeMonth(month()); // this writes at the start of the new month
        // to prevent repetition we opdate datum
        datum = day();
       }
     
       getTijd(); // retrieve time 
  }
  
  // we do this before sending polling info and healthcheck
  // this way it can't block the loop
  if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
 
  //*********************************************************************

  
  test_actionFlag();
  
//  if( Serial.available() ) {
//    empty_serial(); // clear unexpected incoming data
//   }

   ws.cleanupClients();
   yield(); // to avoid wdt resets

//  SERIAL: *************** check if there is data on serial **********************
  if(Serial.available()) {
       handle_Serial();
   }

}
//****************  End Loop   *****************************




// // ****************************************************************
// //                  eeprom handlers
// //*****************************************************************
// void write_eeprom() {
//     EEPROM.begin(24);
//   //struct data
//   struct { 
//     char str[16] = "";
//     //int mont;
//   } data;

//  strcpy( data.str, requestUrl ); 
//     //mont = currentMonth
//     EEPROM.put(0, data);
//     EEPROM.commit();
// }
// void read_eeprom() {
//     EEPROM.begin(24);

//   struct { 
//     char str[16] = "";
//     //int mont;
//   } data;

//   EEPROM.get(0, data);
//   //Serial.println("read value from  is " + String(data.str));
//   strcpy(requestUrl, data.str);
//   //currentMonth = data.mont;
//   EEPROM.commit();
// }



void flush_wifi() {
     consoleOut(F("erasing the wifi credentials"));
     WiFi.disconnect();
     consoleOut("wifi disconnected");
     //now we try to overwrite the wifi credentials     
     const char* wfn = "dummy";
     const char* pw = "dummy";
     WiFi.begin(wfn, pw);
     consoleOut(F("\nConnecting to dummy network"));
     int teller = 0;
     while(WiFi.status() != WL_CONNECTED){
        Serial.print(F("wipe wifi credentials\n"));
        delay(100);         
        teller ++;
        if (teller > 2) break;
    }
    // if we are here, the wifi should be wiped 
}


void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            
  } else 
     if (what == 2) {
     events.send( "getall", "message"); //getAll triggered
  } else {  
     events.send( "reload", "message"); // both triggered
  }
}
// void writeMonth(int maand) {
//   //so if month overflew, the value of end 7 is in 8
// //month goes from 1 - 12 buth the struct from 0 - 12
// // we write all values in the struct with the number of current month -1
//    MVALS[maand].EC_LT = ECON_LT ;
//    MVALS[maand].EC_HT = ECON_HT ;
//    MVALS[maand].ER_LT = ERET_LT ;
//    MVALS[maand].ER_HT = ERET_HT ;
//    MVALS[maand].mGAS    = mGAS;
// // write this in SPIFFS
//    String bestand = "//mvalues_" + String(maand) + ".str"; // month5.str
//    writeStruct(bestand, maand);
// } 

 
