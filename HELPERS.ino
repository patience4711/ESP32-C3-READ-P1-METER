
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

void resetCause(){
  // Read the current reset reason
  esp_reset_reason_t Reason = esp_reset_reason();

  // Start Preferences in RW mode
  prefs.begin("reset_log", false);  // namespace: "reset-info"

  // Save current reset reason
  prefs.putInt("last_reset", (int)Reason);

  // Optional: Read previous reset reason (before the last reset)
  int prevReason = prefs.getInt("prev_reset", -1); // default -1 if not found
  prefs.putInt("prev_reset", (int)Reason);    // Shift current to previous

  // Close preferences
  prefs.end();
}  

void sendHeap() {
// should be something like {"command":"udevice", "idx":1234, "svalue":"12345"}
 
String toMQTT = "{\"idx\":922,\"nvalue\":0,\"svalue\":\"" + String(ESP.getFreeHeap()) + "\"}";
consoleOut("sendHeap toMQTT = " + toMQTT);
if(mqttConnect() ) MQTT_Client.publish ( Mqtt_outTopic, toMQTT.c_str(), false );
}

// * * * test serial port * * * 
void testMessage() {
char * inMessage = {0};
consoleOut("performing loopback test on uart0");
 if (Serial.availableForWrite() > 9 )
    {
        Serial.write("T");
        Serial.write("E");
        Serial.write("S");
        Serial.write("T"); 
        Serial.write("-");
        Serial.write("m");
        Serial.write("e");
        Serial.write("e");                
        Serial.write("\n");
        if (diagNose) ws.textAll("Serial wrote testdata");
    } else {
      consoleOut("Serial not available");
      return;
    }

     //now read the answer
     consoleOut("reading Serial1");
     waitSerial1Available(1);  
     
     if( Serial1.available() ) 
     {
        int inByte = Serial1.read();
        Serial1.write(inByte);
   } else { consoleOut("Serial1 has no data");}
}


// function to show debug info
void consoleOut(String toLog) {
// decide to log to serial or the console 
  if(diagNose) 
  {
    Serial.println(toLog);
  } 
}

void loginBoth(AsyncWebServerRequest *request, String who) {
  //String authFailResponse = "<h2>login failed <a href='/'>click here</a></h2>";
  if (who == "admin" ){
  const char* www_realm = "login as administrator."; 
    if (!request->authenticate("admin", pswd)) return request->requestAuthentication();
  }
  if (who == "both" ){
  const char* www_realm = "login as administrator or user."; 
    if (!request->authenticate("admin", pswd) && !request->authenticate("user", userPwd)) return request->requestAuthentication();
  }
}


String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32C3-" + String(chipId);
}


long getValidVal(long valNew, long valOld, long maxDiffer)
{
  //check if the incoming value is valid
      if(valOld > 0 && ((valNew - valOld > maxDiffer) && (valOld - valNew > maxDiffer)))
        return valOld;
      return valNew;
}

char *split(char *str, const char *delim)
{
    char *p = strstr(str, delim);

    if (p == NULL)
        return NULL; // delimiter not found

    *p = '\0';                // terminate string after head
    return p + strlen(delim); // return tail substring
}


double round3(double value) {
   return (int)(value * 1000 + 0.5) / 1000.0;
}
double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}
double round0(double value) {
   return (int)(value + 0.5) ;
}



void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
    //uart_read_bytes(UART_NUM_2, &data, 1, 20 / portTICK_PERIOD_MS);
  }
}

void empty_Serial1() { // remove any remaining data in serial buffer
  while(Serial1.available()) {
    Serial1.read();
  }
}
//File file = root.openNextFile();
//          int total = 0;
//          while(file){
//             total += file.size();
//             Serial.println("File: " + String(file.name()) + " size: " + String(file.size()) ); 
//             file = root.openNextFile();
void showDir() {
    ws.textAll("**** Content filesystem ****");
    char temp[50]={0};
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
          sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
          ws.textAll(String(temp));
          delay(100);
          file = root.openNextFile();
    }
 }

 void printFiles() {  
      consoleOut(" print existing files ******");
      for (int x=1; x < 13; x++) 
      {
          String bestand = "/monthly_vals" + String(x) + ".str";
          printStruct(bestand, x);
      }
        Serial.println("the current values are:");
        Serial.println("meter.con_lt = " + String(meter.con_lt));     
        Serial.println("meter.con_ht = " + String(meter.con_ht));
        Serial.println("meter.ret_lt = " + String(meter.ret_lt));
        Serial.println("meter.ret_ht = " + String(meter.ret_ht));
        Serial.println("meter.gas = " + String(meter.gas));     
}

void writeMonth(int maand) {
  //so if month overflew, the value of end 7 is in 8
//month goes from 1 - 12 buth the struct from 0 - 12
// we write all values in the struct with the number of current month -1
   MVALS[maand].EC_LT = meter.con_lt ;
   MVALS[maand].EC_HT = meter.con_ht ;
   MVALS[maand].ER_LT = meter.ret_lt ;
   MVALS[maand].ER_HT = meter.ret_ht ;
   MVALS[maand].mGAS    = meter.gas;
// write this in SPIFFS
   String bestand = "/mvalues_" + String(maand) + ".str"; // month5.str
   writeStruct(bestand, maand);
} 
