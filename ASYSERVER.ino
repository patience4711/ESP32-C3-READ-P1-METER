void start_asyserver() {

server.on("/api/v1/data", HTTP_GET, [&](AsyncWebServerRequest *request) {
    //unsigned long startAPI = millis();
    consoleOut("/api/v1/data request via snprintf pointer");
   
    if(ledSignal) my_wifiConnect.ledOn(255,0,0);
    if (templateMade) {
        
        // 1. Bereken de netto live-waarden per fase
        int pwr_l1 = (int)meter.pwr_con[0] - (int)meter.pwr_ret[0];
        int pwr_l2 = (int)meter.pwr_con[1] - (int)meter.pwr_ret[1];
        int pwr_l3 = (int)meter.pwr_con[2] - (int)meter.pwr_ret[2];
        int total_pwr = pwr_l1 + pwr_l2 + pwr_l3;

        // 2. Bepaal waar jsonPart1 eindigt
        size_t index = strlen(jsonApi);

        // 3. Schrijf de live data DIRECT aan het einde van de char-array
        // We gebruiken 'jsonPart1 + index' om de pointer exact op de lege plek te zetten.
        // We bewaken streng de resterende ruimte met 'sizeof(jsonPart1) - index'.
        snprintf(jsonApi + index, sizeof(jsonApi) - index,
            "  \"active_power_w\": %d,\n"
            "  \"active_power_l1_w\": %d,\n"
            "  \"active_power_l2_w\": %d,\n"
            "  \"active_power_l3_w\": %d\n"
            "}",
            total_pwr, pwr_l1, pwr_l2, pwr_l3
        );
        // 4. Zend de resulterende char-array direct uit (0% String overhead!)
        request->send(200, "application/json", jsonApi);

        // 5. CRUCIAAL: Herstel jsonPart1 direct weer naar zijn schone 'open' status!
        // We zetten de null-terminator terug op de oorspronkelijke plek, 
        // zodat de live-data van de volgende request er niet achteraan wordt geplakt.
        jsonApi[index] = '\0';
        //apiDurationMs = millis() - startAPI;

     } else {
        request->send(503, "text/plain", "meter not polled yet...");
     }
     if(ledSignal) my_wifiConnect.ledOff();
});




server.on("/api/v1/power", HTTP_GET, [](AsyncWebServerRequest *request) {

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    //StaticJsonDocument<160> doc; //(160);
    // the length = 17*2 + 10*2 = 52

    JsonDocument root;
    root["active_power_w"]    = (unsigned int)meter.pwr_con; //else root["pac"] = "n/a";
    root["active_power_l1_w"] = (unsigned int)meter.pwr_ret; //else root["par"] = "n/a";
    serializeJson(root, * response);
    request->send(response);
});

server.on("/get.Data", HTTP_GET, [](AsyncWebServerRequest *request) {
    uint8_t remote = 0;
    if(checkRemote(request->client()->remoteIP().toString())) remote = 1;

    if (templateMade) {
        //establish where the jsonWebui Buffer currently ends
        size_t index = strlen(jsonWebui);

        //add the remote-flag and the closing brace
        snprintf(jsonWebui + index, sizeof(jsonWebui) - index,
            ",\n  \"rm\": %d\n}",
            remote
        );

        //send the complete char-array directly
        request->send(200, "application/json", jsonWebui);

        // 4. Herstel de buffer direct weer naar zijn schone, open status voor de volgende request
        jsonWebui[index] = '\0';
    } 
    else {
        // --- De veilige opstart-fallback (Ook 100% String-vrij!) ---
        char fallbackJson[400];
        snprintf(fallbackJson, sizeof(fallbackJson), 
            "{\n"
            "  \"timestamp\": \"--/-- --:--:--\",\n"
            "  \"CON_HT\": 0.000,\n"
            "  \"CON_LT\": 0.000,\n"
            "  \"RET_HT\": 0.000,\n"
            "  \"RET_LT\": 0.000,\n"
            "  \"PWRC1\": 0,\n"
            "  \"PWRR1\": 0,\n"
            "  \"PWRC2\": 0,\n"
            "  \"PWRR2\": 0,\n"
            "  \"PWRC3\": 0,\n"
            "  \"PWRR3\": 0,\n"
            "  \"enR\": 0.000,\n"
            "  \"enC\": 0.000,\n"
            "  \"gAs\": 0.000,\n"
            "  \"threeP\": %d,\n" // We sturen de echte fase-configuratie alvast mee
            "  \"rm\": %d\n"       // De cruciale menuknop vlag!
            "}", 
            threePhase, remote
        );
        request->send(200, "application/json", fallbackJson);
    }
});


server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    request->send_P(200, "text/html", CONSOLE_HTML);
  });


// ***********************************************************************************
//                                     homepage
// ***********************************************************************************
server.on("/SW=BACK", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    request->redirect( String(requestUrl) );
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//    loginBoth(request, "both");
    request->send_P(200, "text/html", P1_HOMEPAGE );
});

server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", STYLESHEET);
});
//server.on("/STYLESHEET_SUBS", HTTP_GET, [](AsyncWebServerRequest *request) {
//    request->send_P(200, "text/css", STYLESHEET_SUBS);
//});
server.on("/JAVASCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", JAVA_SCRIPT);
});
server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", SECURITY);
});

server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println("favicon requested");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", FAVICON, FAVICON_len);
    request->send(response);
});

server.on("/MENU", HTTP_GET, [](AsyncWebServerRequest *request) {
//Serial.println("requestUrl = " + request->url() ); // can we use this
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );

  loginBoth(request, "admin");
  toSend = FPSTR(HTML_HEAD);
  toSend += FPSTR(MENUPAGE);
  //toSend.replace( "{title}" , String(dvName)) ;
  //toSend.replace( "{device}" , String(dvName)) ;
request->send(200, "text/html", toSend);
});

server.on("/DENIED", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", REQUEST_DENIED);
});


// ***********************************************************************************
//                                   basisconfig
// ***********************************************************************************

server.on("/submitform", HTTP_GET, [](AsyncWebServerRequest *request) {
handleForms(request);
confirm(); // puts a response in toSend
request->send(200, "text/html", toSend); // tosend is 
});

server.on("/BASISCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );// remember this to come back after reboot
    zendPageBasis(request);
    //request->send(200, "text/html", toSend);
});

//server.on("/basisconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
//    handleBasisconfig(request);
//    //request->send(200, "text/html", toSend);
//    request->redirect( String(requestUrl) );
//});

server.on("/MQTT", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );
    zendPageMQTTconfig(request);
});

server.on("/SETVALUES", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );
    zendPageVALUEconfig(request);
});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );
    zendPageGEOconfig(request);
});

server.on("/REBOOT", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    actionFlag = 10;
    confirm(); 
    request->send(200, "text/html", toSend);
});

server.on("/STARTAP", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/'; }, 5000 ); </script>");
  toSend += ("</head><body><center><h2>OK wifi settings flushed and the AP is started.</h2>Wait until the led goes on.<br><br>Then open wifi settings on your phone/tablet/pc and connect to ");
  toSend += getChipId(false);
  
  request->send ( 200, "text/html", toSend ); //zend bevestiging
  actionFlag = 11;
});

server.on("/ABOUT", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println(F("/INFOPAGE requested"));
    loginBoth(request, "both");
    handleAbout(request);
});
server.on("/TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    actionFlag = 44;
    request->send( 200, "text/html", "<center><br><br><h3>checking zigbee.. please wait a minute.<br>Then you can find the result in the log.<br><br><a href=\'/PAGE\'>click here</a></h3>" );
});

server.on("/REPORT", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    strcpy( requestUrl, request->url().c_str() );
    //handleReport(request);
    request->send_P(200, "text/html", REPORTPAGE, putReport);
});


// Handle Web Server Events
events.onConnect([](AsyncEventSourceClient *client){
//  if(client->lastId()){
//    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
//  }
});
server.addHandler(&events);



// ********************************************************************
//                    X H T  R E Q U E S T S
//***********************************************************************

// this is for the mDNS discovery
server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request){
  // Zendure checks product_type and firmware_version
  String json = "{\"product_type\":\"HWE-P1\",\"product_name\":\"P1 Meter\",\"firmware_version\":\"4.19\"}";
  request->send(200, "application/json", json);
});

// ***************************************************************************************
//                           Simple Firmware Update
// ***************************************************************************************                                      
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    //program = 10; // we should shut off otherwise we can't reboot
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    strcpy(requestUrl, "/");
    if (!request->authenticate("admin", pswd) ) return request->requestAuthentication();
    request->send_P(200, "text/html", otaIndex); 
    });
  server.on("/handleFwupdate", HTTP_POST, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    Serial.println("FWUPDATE requested");
    if( !Update.hasError() ) {
    toSend="<br><br><center><h2>UPDATE SUCCESS !!</h2><br><br>";
    toSend +="click here to reboot<br><br><a href='/REBOOT'><input style='font-size:3vw;' type='submit' value='REBOOT'></a>";
    } else {
    toSend="<br><br><center><kop>update failed<br><br>";
    toSend +="click here to go back <a href='/FWUPDATE'>BACK</a></center>";
    }
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", toSend);
    response->addHeader("Connection", "close");
    request->send(response);
  
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Serial.println("filename = " + filename);
    if(filename != "") {
    if(!index){
      //#ifdef DEBUG
        Serial.printf("start firmware update: %s\n", filename.c_str());
      //#endif
      //Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        //#ifdef DEBUG
          Update.printError(Serial);
        //#endif
      }
    }
    } else {
      consoleOut("filename empty, aborting");
//     Update.hasError()=true;
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
          Serial.println("update failed with error: " );
          Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("firmware Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });
// if everything failed we come here
server.onNotFound([](AsyncWebServerRequest *request){
  //Serial.println("unknown request");
  handleNotFound(request);
});

server.begin(); 
}

//void confirm() {
//toSend="<html><body onload=\"setTimeout(function(){window.location.href='/';}, 3000 );\"><br><br><center><h3>processing<br>your request,<br>please wait</html>";
//}
void confirm() {
toSend="<html><body onload=\"setTimeout(function(){window.location.href='";
toSend+=String(requestUrl);
toSend+="';}, 3000 );\"><br><br><center><h1>processing<br>your request,<br>please wait</h1></html>";
//Serial.println(toSend);
}
