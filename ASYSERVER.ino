void start_asyserver() {

// when there is a pollfreq below 10, the meter is polled if higher then the request
// answers with automatically polled data
server.on("/api/v1/data", HTTP_GET, [](AsyncWebServerRequest *request)
{
    // this link provides the data on the api request
    consoleOut("answer a api request");
    // if there is a high poll frequency we poll the meter when the poll is 3, 4, 5, 6, 7 seconds old
    if (pollFreq >= 1 && pollFreq <= 10) 
    {
      unsigned long currentTime = millis();
      // Bereken de dynamische cooldown in milliseconden (bijv. 3 * 1000 = 3000ms)
      unsigned long cooldownMs = pollFreq * 1000;
       
      if (currentTime - laatsteMeting > cooldownMs || laatsteMeting == 0) {
          maxTry = 1;
          meterPoll(); 
          laatsteMeting = currentTime; 
      }     
    }
    
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument root; //(length current 291);
   
    root["smr_version"] = 50;
    root["meter_model"] = String(meterType); // moet
    root["wifi_ssid"] = WiFi.SSID();
    root["wifi_strength"] = WiFi.RSSI();
    root["total_power_import_t1_kwh"] = round3(meter.con_ht); // tariff 1
    root["total_power_import_t2_kwh"] = round3(meter.con_lt); // tariff 2
    root["total_power_export_t1_kwh"] = round3(meter.ret_ht); // tariff 1
    root["total_power_export_t2_kwh"] = round3(meter.ret_lt); // tariff 2  
   
    long total_power_w = 0; // temporary power val
    if (threePhase)
    {
        // 3-phase meter: calculate the sum of all 3 phases [0, 1, 2]
        long l1_net = round0(meter.pwr_con[0] - meter.pwr_ret[0]);
        long l2_net = round0(meter.pwr_con[1] - meter.pwr_ret[1]);
        long l3_net = round0(meter.pwr_con[2] - meter.pwr_ret[2]);
        // the total power is simply the addition of the power per phasee
        total_power_w = l1_net + l2_net + l3_net;
        // Vul de fasespecifieke JSON-velden in voor HomeWizard/Zendure
        root["active_power_l1_w"] = l1_net;
        root["active_power_l2_w"] = l2_net;
        root["active_power_l3_w"] = l3_net;
    }
    else
    {
        // 1-Fase meter: only phase 1 [0] has data
        total_power_w = round0(meter.pwr_con[0] - meter.pwr_ret[0]);
        //with a 1-phase meter, L1 is the total
        root["active_power_l1_w"] = total_power_w;
        // Optional: Some systems want to see also L2 and L3 with 1-fase
        root["active_power_l2_w"] = 0;
        root["active_power_l3_w"] = 0;
    }
    // This is the main field that Zendure checks to charge / discharge the battery
    root["active_power_w"] = total_power_w;
    // --------------------------------------------------------
   
    String jsonString;
    serializeJson(root, * response);
    // Final heap check
    actionFlag = 130;
    request->send(response);  
});

server.on("/api/v1/power", HTTP_GET, [](AsyncWebServerRequest *request) {
    // if there is a high poll frequency we poll the meter when the poll is 3, 4, 5, 6, 7 seconds old
      if (pollFreq >= 1 && pollFreq <= 10) {
      unsigned long currentTime = millis();
      // Bereken de dynamische cooldown in milliseconden (bijv. 3 * 1000 = 3000ms)
      unsigned long cooldownMs = pollFreq * 1000;
       
      if (currentTime - laatsteMeting > cooldownMs || laatsteMeting == 0) {
          maxTry = 1;
          meterPoll(); 
          laatsteMeting = currentTime; 
      }     
    }

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
// this link provides the general data on the frontpage
    char temp[13]={0};
    uint8_t remote = 0;
    if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link

// {"ps":"05:27 hr","pe":"21:53 hr","cnt":3,"rm":0,"st":1,"sl":1}  length 62
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    //StaticJsonDocument<160> doc; //(160);
    JsonDocument root; //(160);
 
    float enReturn = meter.ret_ht + meter.ret_lt;
    float enCons   = meter.con_ht + meter.con_lt;
    //uint16_t Power = meter.pwr_con[0] - meter.pwr_ret[0] + meter.pwr_con[1] - meter.pwr_ret[1] + meter.pwr_con[2] - meter.pwr_ret[2];
    
    
    root["timestamp"] = String(timeStamp);

    root["CON_HT"] = round3(meter.con_ht);
    root["CON_LT"] = round3(meter.con_lt); 
    
    root["RET_HT"] = round3(meter.ret_ht);
    root["RET_LT"] = round3(meter.ret_lt);
    
    root["PWRC1"] = round0(meter.pwr_con[0]); 
    root["PWRR1"] = round0(meter.pwr_ret[0]);
    root["PWRC2"] = round0(meter.pwr_con[1]); 
    root["PWRR2"] = round0(meter.pwr_ret[1]);
    root["PWRC3"] = round0(meter.pwr_con[2]); 
    root["PWRR3"] = round0(meter.pwr_ret[2]);
    
    root["enR"] = round3(enReturn); 
    root["enC"] = round3(enCons); 
    //root["aPo"] = round0(Power); 
    root["gAs"] = round3(meter.gas); 
    root["threeP"] = threePhase; 
    root["rm"] = remote;
   
    serializeJson(root, * response);
    request->send(response);
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
