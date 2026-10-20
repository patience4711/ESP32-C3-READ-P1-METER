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
       } else
       
        if ( serverUrl.indexOf("/READINGS") > -1) {
            // Start th HTML page with minimal styling for the table
            String html = "<html><head><meta name='viewport' content='width=width, initial-scale=1.0'>";
            html += "<style>";
            html += "body { font-family: Arial, sans-serif; background-color: #f4f4f9; padding: 20px; }";
            html += "table { width: 100%; max-width: 600px; border-collapse: collapse; background: white; margin: 0 auto; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
            html += "th, td { padding: 5px 8px; text-align: left; border-bottom: 1px solid #ddd; }";
            html += "th { background-color: #4CAF50; color: white; }";
            html += "tr:nth-child(even) { background-color: #f2f2f2; }";
            html += "h2 { text-align: center; color: #333; }";
            html += "</style></head><body>";
            
            html += "<h2>Monthly Meterreadings</h2>";
            html += "<table>";
            html += "<tr><th>Month</th><th>cons LT (kWh)</th><th>cons HT(kWh)</th><th>Return LT (kWh)</th><th>Return HT (kWh)</th><th>Gas (m3)</th></tr>";

            // walk through the months 1 to 12
            for (int m = 1; m <= 12; m++) {
               html += "<tr>";
               html += "<td><b>" + String(m) + "</b></td>";
               html += "<td>" + String(MVALS[m].EC_LT, 3) + "</td>";
               html += "<td>" + String(MVALS[m].EC_HT, 3) + "</td>";
               html += "<td>" + String(MVALS[m].ER_LT, 3) + "</td>";
               html += "<td>" + String(MVALS[m].ER_HT, 3) + "</td>";
               html += "<td>" + String(MVALS[m].mGAS, 3) + "</td>";
               html += "</tr>";
            }
            html += "</table></body></html>";

            request->send(200, "text/html", html);
            return;
      } else

      if ( serverUrl.indexOf("/GETMONTH") > -1) {
            int m = 1;
            if (request->hasParam("mnr")) {
               m = request->arg("mnr").toInt();
            }
            
            // Veiligheidscheck voor de array-grenzen
            if (m < 1 || m > 12) m = 1;

            JsonDocument doc;
            doc["econht"] = MVALS[m].EC_HT;
            doc["econlt"] = MVALS[m].EC_LT;
            doc["eretht"] = MVALS[m].ER_HT;
            doc["eretlt"] = MVALS[m].ER_LT;
            doc["mgas"]   = MVALS[m].mGAS;

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
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
