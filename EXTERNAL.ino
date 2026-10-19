// find out if the request comes from inside the network
//check if the first 9 characters of the router's ip ( 192.168.0 ) is in the url
bool checkRemote(String url) {
   if(securityLevel==0) return false; //no security so never remote
   if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done

void handleNotFound(AsyncWebServerRequest *request) {
  
bool intern = false;
if(!checkRemote( request->client()->remoteIP().toString()) ) intern = true;

// **************************************************************************
//             R E S T R I C T E D   A R E A
// **************************************************************************
// access only from inside the network

if ( intern ) 
    {   consoleOut("the request comes from inside the network");
        String serverUrl = request->url().c_str();             
        consoleOut("external: the serverurl = " + serverUrl);
        if ( serverUrl.indexOf("/API/TELEGRAM") > -1) {
            //char tozend[1024];
            strcat(teleGram, "\r\npolled at ");
            strcat(teleGram, timeStamp);
            //testTelegram = false; // useless for testing   
            request->send(200, "text/plain", String(teleGram) );
            return; //otherwise we get the this link is invalid
        }

        if ( serverUrl.indexOf("/API/poll") > -1) {
              if(pollFreq != 0)
              {
                 request->send ( 200, "text/plain", "automatic polling, skipping" ); //zend bevestiging
                 return; 
              }
             String teZenden = F("polled p1 meter ");
             request->send ( 200, "text/plain", teZenden ); 
             actionFlag = 26; // takes care for the polling
             return;
       }
       
      //  if ( serverUrl.indexOf("/get.ResetReason") > -1) {
      //   //consoleOut("reset check");
      //   prefs.begin("reset_log", false); //namespace name is reset_log
      //   int lastReset = prefs.getInt("reason", -1);
      //   Serial.print("Last saved reset reason: ");
      //   esp_reset_reason_t reason = esp_reset_reason();
      //   String r_r = "last reset reason = " + String(lastReset);
      //   r_r += "\ncurrent reset reason = " + String(reason);
      //   r_r += "\nCode Meaning \n1 Power-on reset \n2 External reset\n3 Software reset";
      //   r_r += "\n4 Watchdog timer reset\n5 Deep sleep wakeup\n6 Brownout reset";
      //   prefs.end();
      //   request->send(500, "text/plain", r_r );
      //   return;
         
      //  }
//                               
//            //char temp[13]={0};
//            //uint8_t remote = 0;
//            //remote = 1; // for the menu link
//            AsyncResponseStream *response = request->beginResponseStream("application/json");
//            DynamicJsonDocument root(512); //(length current 291);
//   
//    root["smr_version"] = 50;
//    root["meter_model"] = "XS210";
//    root["wifi_ssid"] = "hansiart";
//    root["wifi_strength"] = 108;
//    root["total_power_import_t1_kwh"] = round3(ECON_HT);
//    root["total_power_import_t2_kwh"] = round3(ECON_LT);
//    root["total_power_export_t1_kwh"] = round3(ERET_HT);
//    root["total_power_export_t2_kwh"] = round3(ERET_LT);    
//    root["active_power_w"]    = round0(PACTUAL_CON);
//    root["active_power_l1_w"] = round0(PACTUAL_RET);
//    String jsonString;
//    serializeJson(root, * response);
//    request->send(response);
//    // Final heap check
//    Serial.printf("[API] Free heap after request->send: %u bytes\n", ESP.getFreeHeap());
//    return;
//   }
// if we are here, no valid api was found    
   request->send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
   
   } else { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
    }

}
