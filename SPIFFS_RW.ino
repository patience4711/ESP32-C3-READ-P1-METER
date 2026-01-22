// ******************   spiffs lezen  *************************

// als er geen spiffs bestand is dan moet hij eigenlijk altijd een ap openenen
void SPIFFS_read() {
          //DebugPrintln("mounting FS...");
          if (!SPIFFS.begin(true)) {
          consoleOut("could not mount file system");
          return;
       }
       consoleOut("reading SPIFFS");
//       if( file_open_for_read("/logChar.txt") ) {
//                Serial.println("\nread logChar.txt\n");
//          } else {
//             Serial.println("\nlogChar.txt not opened\n");
//          }
       if( file_open_for_read("/testFile.txt") ) {
                Serial.println("\nread testFile\n");
                testTelegram = true;
          } else {
             Serial.println("\ntestFile.txt not opened\n");
          }
       if( file_open_for_read("/wificonfig.json") ) {
                Serial.println("read wificonfig\n");
          } else {
             Serial.println("wificonfig.json not opened\n");
          }
       
       if( file_open_for_read("/basisconfig.json") ) {     
             Serial.println("read basisconfig\n");
          } else {
          Serial.println("basisconfig.json not opened\n");
        } 
       if( file_open_for_read("/mqttconfig.json") ) {     
             Serial.println("mqttconfig read");
          } else {
          Serial.println("mqttconfig.json not opened");
        }         
  
  //} else {    consoleOut("mounted file system"); }
 // einde spiffs lezen 3 bestanden

}  

// writeStruct(bestand, month); // alles opslaan in SPIFFS

void writeStruct( String whichfile, int mnd) {
      File configFile = SPIFFS.open(whichfile, "w");
      // f.i. monthly_vals0.str
        if (!configFile)
           {
             Serial.print(F("Failed open for write : ")); Serial.println(whichfile);            
           } 
             Serial.print(F("Opened for write....")); Serial.println(whichfile);
             configFile.write( (unsigned char *)&MVALS[mnd], sizeof(MVALS[mnd]) );
             configFile.close();
}

bool readStruct(String whichfile ,int mnd) {
      //input = //mvalues_0.str
      File configFile = SPIFFS.open(whichfile, "r");

      Serial.print(F( "readStruct mnd = ")); Serial.println( String(mnd) );  
        if (!configFile)
           {
              Serial.print(F("Failed to open for read")); Serial.println(whichfile);
              return false;           
           } 
              Serial.print(F("reading ")); Serial.println(whichfile);
              configFile.read( (unsigned char *)&MVALS[mnd], sizeof(MVALS[mnd]) );
              configFile.close();
              return true;
 }



// **************************************************************************** 
//             save data in SPIFFS                         *  
// ****************************************************************************

void testFilesave() {
   //DebugPrintln("saving config");
    if( SPIFFS.exists("/testFile.txt") ){
      Serial.println("testfile exists, we don't overwrite it");
      return;
    }
   JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();   
    json["content"] = teleGram;

    File configFile = SPIFFS.open("/testFile.txt", "w");
    serializeJson(json, Serial);
    Serial.println(F("")); 
    serializeJson(json, configFile);
    configFile.close();
}



void wifiConfigsave() {
   //DebugPrintln("saving config");

    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();   
//    json["ip"] = static_ip;
    json["pswd"] = pswd;
    Serial.println("spiffs save pswd = " + String(pswd));
    json["longi"] = longi;
    json["lati"] = lati;
    json["gmtOffset"] = gmtOffset;
    json["zomerTijd"] = zomerTijd;
    Serial.println("spiffs save securityLevel = " + String(securityLevel));
    json["securityLevel"] = securityLevel;
    File configFile = SPIFFS.open("/wificonfig.json", "w");
    if (!configFile) Serial.println ("open wificonfig.json failed");
    if(diagNose){ 
      serializeJson(json, Serial);
      Serial.println("");     
    }
    Serial.println(F("")); 
    serializeJson(json, configFile);
    configFile.close();
}


void basisConfigsave() {
    //DebugPrintln("saving basis config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();
    json["userPwd"] = userPwd;
    json["meterType"] = meterType;
    json["bootTest"] = bootTest;
    json["baudRate9600"] = baudRate9600;
    json["pollFreq"] = pollFreq;
    json["threePhase"] = threePhase;
    json["rxInvert"] = rxInvert;
    json["diagNose"] = diagNose;    
    File configFile = SPIFFS.open("/basisconfig.json", "w");
    if (!configFile) {
    Serial.println("open basisconfig failed");
    }
    if(diagNose){ 
      serializeJson(json, Serial);
      Serial.println("");     
    }
    serializeJson(json, configFile);
    configFile.close();
    }

void mqttConfigsave() {
   //DebugPrintln("saving mqtt config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();

    json["Mqtt_Broker"] = Mqtt_Broker;
    json["Mqtt_Port"] = Mqtt_Port;    
    json["gas_Idx"] = gas_Idx;
    json["el_Idx"] = el_Idx;
    json["Mqtt_outTopic"] = Mqtt_outTopic;
    json["Mqtt_inTopic"] = Mqtt_inTopic;
    json["Mqtt_Username"] = Mqtt_Username;
    json["Mqtt_Password"] = Mqtt_Password;
    json["Mqtt_Clientid"] = Mqtt_Clientid;    
    json["Mqtt_Format"] = Mqtt_Format;    
    File configFile = SPIFFS.open("/mqttconfig.json", "w");
    if (!configFile) {
    Serial.println("open mqttconfig failed");
    }
    if(diagNose){ 
      serializeJson(json, Serial);
      Serial.println("");     
    }
    serializeJson(json, configFile);
    configFile.close();
}



bool file_open_for_read(const char* bestand) 
{
      Serial.print("we are in file_open_for_read, bestand = "); Serial.println(bestand); 
      if (!SPIFFS.exists(bestand)) return false;
      JsonDocument doc;
      String Output = "";
      //file exists, reading and loading
       
        File configFile = SPIFFS.open(bestand, "r");
        if (configFile) 
        {
        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();
        if (error) 
            {
                Serial.print(F("Failed to parse config file: "));
                Serial.println(error.c_str());
                // Continue with fallback values
            } else {
            // no error so we can print the file
                //serializeJson(doc, Serial);  // always print
                serializeJson(doc, Output);  // always print
                Serial.println("here we should see the file content");
                Serial.println(Output);
            }
    } else {
                Serial.print(F("Cannot open config file: "));
                Serial.println(bestand);
                // Continue with empty doc -> all fallbacks will be used
            }
            if (strcmp(bestand, "/testFile.txt")==0)
                     {
                        strcpy(teleGram, doc["content"] | "spiffs");
                     }
            
            
            // we read the file even if it doesn't exist, so that variables are initialized
            // we read every variable with a fall back value to prevent crashes
            if (strcmp(bestand, "/wificonfig.json") == 0) {
                      //if(jsonStr.indexOf("ip") > 0){ strcpy(static_ip, doc["ip"]);}
                      strcpy(pswd, doc["pswd"] | "0000");
                      longi = doc["longi"] |  5.734;
                      lati = doc["lati"]  |   51.432;                     
                      strcpy(gmtOffset, doc["gmtOffset"] | "+120");
                      zomerTijd = doc["zomerTijd"].as<bool>() | true;
                      securityLevel = doc["securityLevel"].as<int>() | 6;
                      //Serial.println("spiffs securityLevel = " + String(securityLevel));
            }

            if (strcmp(bestand, "/basisconfig.json") == 0) {
                     strcpy (userPwd, doc["userPwd"] | "1111");
                     //if(jsonStr.indexOf("dom_Address")   >  0 ) { strcpy(dom_Address,   doc["dom_Address"])         ;}
                     //if(jsonStr.indexOf("dom_Port") >  0 ) { dom_Port = doc["dom_Port"].as<int>() ;} 
                     bootTest = doc["bootTest"].as<bool>() | false;
                     threePhase = doc["threePhase"].as<bool>() | false;
                     baudRate9600 = doc["baudRate9600"].as<bool>() | false;
                     rxInvert = doc["rxInvert"].as<bool>() | true;
                     meterType = doc["meterType"].as<int>() | 1;
                     pollFreq = doc["pollFreq"].as<int>() | 0;
                     diagNose = doc["diagNose"].as<bool>() | true;
              }            

            if (strcmp(bestand, "/mqttconfig.json") == 0) {
                    strcpy(Mqtt_Broker,   doc["Mqtt_Broker"]   | "192.168.0.100");
                    strcpy(Mqtt_Port,     doc["Mqtt_Port"]     | "1883");  
                    strcpy(Mqtt_outTopic, doc["Mqtt_outTopic"] | "domoticz/in"); 
                    strcpy(Mqtt_inTopic,  doc["Mqtt_inTopic"]  | "domoticz/out");        
                    strcpy(Mqtt_Username, doc["Mqtt_Username"] | "n/a");
                    strcpy(Mqtt_Password, doc["Mqtt_Password"] | "n/a");
                    
                    Mqtt_Format =         doc["Mqtt_Format"].as<int>() | 1;
                    gas_Idx =             doc["gas_Idx"].as<int>() | 100;         
                    el_Idx =              doc["el_Idx"].as<int>() | 100;
            }

              return true;
 }

// we do this before swap_to_zigbee
//void printStruct( String bestand ) {
////input String bestand = "/Inv_Prop" + String(x) + ".str";
//      //String bestand = bestand + String(i) + ".str"
//      //leesStruct(bestand); is done at boottime
////      #ifdef DEBUG
////      int ivn = bestand.substring(9,10).toInt();
////      Serial.println("Inv_Prop[" + String(ivn) + "].invLocation = " + String(Inv_Prop[ivn].invLocation));     
////      Serial.println("Inv_Prop[" + String(ivn) + "].invSerial = " + String(Inv_Prop[ivn].invSerial));
////      Serial.println("Inv_Prop[" + String(ivn) + "].invID = " + String(Inv_Prop[ivn].invID)); 
////      Serial.println("Inv_Prop[" + String(ivn) + "].invType = " + String(Inv_Prop[ivn].invType));
////      Serial.println("Inv_Prop[" + String(ivn) + "].invIdx = " + String(Inv_Prop[ivn].invIdx));
////      Serial.println("Inv_Prop[" + String(ivn) + "].conPanels = " + String(Inv_Prop[ivn].conPanels[0])  + String(Inv_Prop[ivn].conPanels[1]) + String(Inv_Prop[ivn].conPanels[2]) + String(Inv_Prop[ivn].conPanels[3]));      
////      Serial.println("");
////      Serial.println("****************************************");
////      #endif
//}
