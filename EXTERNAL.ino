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
       
       if ( serverUrl.indexOf("/READINGS") > -1) 
       {
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
            for (int m = 1; m <= 12; m++) 
            {
               if( m == month()) {
                  html += "<tr>";
                  html += "<td><b>" + String(m) + "</b></td>";
                  html += "<td>" + String(meter.con_lt, 3) + "</td>";
                  html += "<td>" + String(meter.con_ht, 3) + "</td>";
                  html += "<td>" + String(meter.ret_lt, 3) + "</td>";
                  html += "<td>" + String(meter.ret_ht, 3) + "</td>";
                  html += "<td>" + String(meter.gas, 3) + "</td>";
                  html += "</tr>";    
               } else {
                  html += "<tr>";
                  html += "<td><b>" + String(m) + "</b></td>";
                  html += "<td>" + String(MVALS[m].EC_LT, 3) + "</td>";
                  html += "<td>" + String(MVALS[m].EC_HT, 3) + "</td>";
                  html += "<td>" + String(MVALS[m].ER_LT, 3) + "</td>";
                  html += "<td>" + String(MVALS[m].ER_HT, 3) + "</td>";
                  html += "<td>" + String(MVALS[m].mGAS, 3) + "</td>";
                  html += "</tr>";
               }
            }
            html += "</table></body></html>";
            AsyncWebServerResponse *response = request->beginResponse(200, "text/html", html);
            response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
            request->send(response);
            return;
      } else
      if ( serverUrl.indexOf("/CHECKMONTH") > -1) {
            int m = 1;
            if (request->hasParam("mnr")) {
               m = request->arg("mnr").toInt();
            }
            JsonDocument doc;
            // Veiligheidscheck voor de array-grenzen
            if (m < 1 || m > 12) m = 1;
            if (m == month() ) {
            doc["meter_econlt"] = meter.con_lt;
            doc["meter_econht"] = meter.con_ht;
            doc["meter_eretlt"] = meter.ret_lt;
            doc["meter_eretht"] = meter.ret_ht;
            doc["meter.mgas"]   = meter.gas;  
            
            doc["MVALS_econlt"] = MVALS[m].EC_LT;
            doc["MVALS_econht"] = MVALS[m].EC_HT;
            doc["MVALS_eretlt"] = MVALS[m].ER_LT;
            doc["MVALS_eretht"] = MVALS[m].ER_HT;
            doc["MVALS_mgas"]   = MVALS[m].mGAS;
            
            } else {
            doc["MVALS_econlt"] = MVALS[m].EC_LT;
            doc["MVALS_econht"] = MVALS[m].EC_HT;
            doc["MVALS_eretlt"] = MVALS[m].ER_LT;
            doc["MVALS_eretht"] = MVALS[m].ER_HT;
            doc["MVALS_mgas"]   = MVALS[m].mGAS;
            }
            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
            return;
      } else
      
      if ( serverUrl.indexOf("/GETMONTH") > -1) {
            int m = 1;
            if (request->hasParam("mnr")) {
               m = request->arg("mnr").toInt();
            }
            JsonDocument doc;
            // Veiligheidscheck voor de array-grenzen
            if (m < 1 || m > 12) m = 1;
            if (m == month() ) {
            doc["econlt"] = meter.con_lt;
            doc["econht"] = meter.con_ht;
            doc["eretlt"] = meter.ret_lt;
            doc["eretht"] = meter.ret_ht;
            doc["mgas"]   = meter.gas;  
            } else {
            doc["econlt"] = MVALS[m].EC_LT;
            doc["econht"] = MVALS[m].EC_HT;
            doc["eretlt"] = MVALS[m].ER_LT;
            doc["eretht"] = MVALS[m].ER_HT;
            doc["mgas"]   = MVALS[m].mGAS;
            }
            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
            return;
            }
      
  
   request->send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
   
   } else { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
    }

}
