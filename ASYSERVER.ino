void start_asyserver() {

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

//server.on("/MQTTconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
//handleMQTTconfig(request);
//request->redirect( String(requestUrl) );
//});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );
    zendPageGEOconfig(request);
});

//server.on("/geoconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
//    //DebugPrintln(F("geoconfig requested"));
//    handleGEOconfig(request);
//    request->redirect( String(requestUrl) );
//});

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

 
// ********************************************************************




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

server.on("/api/v1/data", HTTP_GET, [](AsyncWebServerRequest *request) 
{
    // this link provides the data on the api request
    consoleOut("answer a api request");
    // Log heap before processing
    //Serial.printf("[API] Free heap before processing: %u bytes\n", ESP.getFreeHeap());
    // errorcheck

    // if (!request->client()) {
    //     request->send(500, "application/json", "{\"error\":\"client not available\"}");
    //     return;
    // }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument root; //(length current 291);
    
    root["smr_version"] = String(meter.smr);
    root["meter_model"] = meterType;
    root["wifi_ssid"] = "WiFi.SSID()";
    root["wifi_strength"] = WiFi.RSSI();
    root["total_power_import_t1_kwh"] = round3(meter.con_ht); // tariff 1
    root["total_power_import_t2_kwh"] = round3(meter.con_lt); // tariff 2
    root["total_power_export_t1_kwh"] = round3(meter.ret_ht); // tariff 1
    root["total_power_export_t2_kwh"] = round3(meter.ret_lt); // tariff 2   
    // bower balance calculations
    float pwr_l1 = meter.pwr_con[0] - meter.pwr_ret[0];
    float pwr_l2 = meter.pwr_con[1] - meter.pwr_ret[1];
    float pwr_l3 = meter.pwr_con[2] - meter.pwr_ret[2];
    float pwr_tot = pwr_l1 + pwr_l2 + pwr_l3; 
    
    // in a 1 phase meter active_power_w is equal to active_power_l1_w
    root["active_power_w"]    = round0(pwr_tot); // balance of ret and con of all 3 phases
    root["active_power_l1_w"] = round0(pwr_l1);  // balance of ret & con
    if(threePhase)
        {
          root["active_power_l2_w"]  = round0(pwr_l2); // balance of ret & con
          root["active_power_l3_w"]  = round0(pwr_l3); // balance of ret & con
        }
    
    String jsonString;
    serializeJson(root, * response);

    // Final heap check
    actionFlag = 130;
    request->send(response);  
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

void confirm() {
//if(device) snprintf(requestUrl, sizeof(requestUrl), "/DEV?welke=%d", devChoice);
toSend  = "<html><head><script>";
toSend += "let waitTime=" + String(3000*procesId) + ";";
toSend += "function redirect(){";
toSend += " let counter=document.getElementById('counter');";
toSend += " let secs=waitTime/1000;";
toSend += " counter.textContent=secs;";
toSend += " let timer=setInterval(function(){";
toSend += "   secs--; counter.textContent=secs;";
toSend += "   if(secs<=0){ clearInterval(timer); window.location.href='" + String(requestUrl) + "'; }";
toSend += " },1000);";
toSend += "}";
toSend += "</script></head>";
toSend += "<body onload='redirect()'>";
toSend += "<br><br><center><h3>processing<br>your request,<br>please wait<br><br>";
toSend += "Redirecting in <span id='counter'></span> seconds...</h3></center>";
toSend += "</body></html>";
}
