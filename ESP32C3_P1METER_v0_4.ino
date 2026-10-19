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

2025-06-26 it seems thst the esp resets after some daya, memory leak?
*/

#include "AAA-INCLUDES.h"

// *****************************************************************************
// *                              SETUP
// *****************************************************************************


#define RXP1 3
#define TXP1 2
//#define GPS_BAUD 115200
int failCount = 0;
void setup() {

  Serial1.setRxBufferSize(1024); 
  // with a large buffer setting i get a lot of malformed telegrams
  // or should i look again to the value of the resistor
  Serial1.begin(115200, SERIAL_8N1, 3, 2); 
  // is 5v logic level too much? checked, didn't help
  // mde the rx input pullup didn't help
  //uart_set_line_inverse(UART_NUM_1, UART_SIGNAL_RXD_INV);
  //pinMode(3, INPUT_PULLUP);
  delay(100);
  Serial.begin(115200);
  Serial.println("The device started");
  Serial.println("Serial1 started at 115200 baud rate");
  resetCause(); // logs the reasons for resets
  pinMode(knop, INPUT_PULLUP); // the button gpio4 D2 
  //pinMode(led_onb, OUTPUT); // onboard led gpio0 D3
  pinMode(P1_ENABLE, OUTPUT);// with this pin gpio14 (D5)  
  digitalWrite(P1_ENABLE, LOW);
   my_wifiConnect.ledBlink(0,150,0,1,800);
  
  attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);

  SPIFFS_read();
  if(rxInvert) uart_set_line_inverse(UART_NUM_1, UART_SIGNAL_RXD_INV);
  
  my_wifiConnect.start_wifi();
  // ledBlink(0, 0, 150, 3, 250); // blink blue
  Serial.println(WiFi.localIP());
  delay(100);
  start_asyserver();

  getTijd(); // retrieve time from the timeserver

// reed monthly values files
  for (int x=1; x < 13; x++) 
  {
  String bestand = "/mvalues_" + String(x) + ".str";  
       readStruct(bestand, x);
}

//  // ****************** mqtt init *********************
       MQTT_Client.setKeepAlive(150);
       MQTT_Client.setBufferSize(512);
       MQTT_Client.setServer(Mqtt_Broker, Mqtt_Port );
       MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
  else 
  {
   consoleOut("mqtt not enabled"); 
  }
  initWebSocket();

  Serial.println(F("booted up"));
  Serial.println(WiFi.localIP());

  delay(1000);
  //ledblink(3,500);

  eventSend(0);
  if ( !timeRetrieved )   getTijd();

  if(pollFreq > 10) meterPoll(); //we do the first poll

  

} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {

// if (WiFi.status() != WL_CONNECTED) {
//   consoleOut("wifi was disconnected, reconnecting");
//   start_wifi(); // ???
//   //start_wifi();
// }
//if(moetZenden) send_testGram();
//              polling every 60 seconds 
// ******************************************************************

   
   if(pollFreq > 10)
   {
      unsigned long nu = millis();  // the time the program is running
      if (nu - laatsteMeting >= 1000UL * pollFreq) 
      {
        if(diagNose) {
        String toLog = String(pollFreq) + " seconds passed";
          ws.textAll( toLog );
        }
          laatsteMeting += 1000UL * pollFreq ; // 
          // the p1 meter only transmits when the rx line = high
          //digitalWrite(P1_ENABLE, HIGH);
          maxTry = 5;
          meterPoll(); 
          //digitalWrite(P1_ENABLE, LOW);
      }
    } else {
    if(pollFreq != 0) {
        unsigned long currentTime = millis();
        // if there hasn't been a request for 30 sec, then poll anyway 
        // so that the webui isn't too outdated
        if (currentTime - laatsteMeting > 10000) {
            maxTry = 5;
            meterPoll();
            laatsteMeting = currentTime;
          }
        }
    }

   
  // when there is a new date the time is retrieved
  // if retrieve fails, day will not be datum, so we keep trying
  if (day() != datum) // if date overflew 
  { 
       if(day() < datum) { // means its the 1st of the new month 
        // the struct goes from 0 - 11
        writeMonth(month()); // this writes at the start of the new month
        // to prevent repetition we update datum
        datum = day();
       }
     
       getTijd(); // retrieve time and recalculate the switch times
  }
  
  // we do this before sending polling info and healthcheck
  // this way it can't block the loop
  if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
 
  //*********************************************************************

  
  test_actionFlag();
  
//  SERIAL: *************** check if there is data on serial **********************
  if(Serial.available()) {
       handle_Serial();
   }

}
//****************  End Loop   *****************************





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





 
