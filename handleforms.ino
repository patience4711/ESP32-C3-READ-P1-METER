void handleForms(AsyncWebServerRequest *request) {
     //every form submission is handled here basisconfig geoconfig mqtt-config
     // we find out which form with a parameter present 
     String serverUrl = request->url().c_str();
     Serial.println("serverUrl = " + serverUrl); // this is /submitform 

     if(request->hasParam("mtype")) {
         // received form basisconfig
         strcpy(userPwd, request->arg("pw1").c_str());
         meterType   =          request->arg("mtype").toInt(); //values are 0 1 2
         pollFreq   =           request->arg("pfreq").toInt(); //values are 0 30 60 300
         //if(request->hasParam("pL")) Polling = true; else Polling = false;
         if(request->hasParam("debug")) diagNose = true;  else diagNose = false;
         if(request->hasParam("tst")) bootTest = true;  else bootTest = false;
         if(request->hasParam("3ph")) threePhase = true;  else threePhase = false;
         if(request->hasParam("baud")) baudRate9600 = true;  else baudRate9600 = false;
         if(request->hasParam("rxI")) rxInvert = true;  else rxInvert = false;
         basisConfigsave();  // alles opslaan
         return; 
     } else 

      if(request->hasParam("longi")) {
        // received the geoconfig form
        longi = request->getParam("longi")->value().toFloat();
        lati = request->getParam("be")->value().toFloat();
        strcpy(gmtOffset, request->getParam("tz")->value().c_str());
        // a checkbox has only a parameter when checked so we check only "is there a param"
        if(request->hasParam("ts")) DTS = true;  else  DTS = false;
        wifiConfigsave();
        actionFlag=25; // recalculate with these settings 
        return;
     } else
     
     if(request->hasParam("mqtAdres")) {
        // form mosquitto received
        strcpy( Mqtt_Broker  , request->getParam("mqtAdres")   ->value().c_str() );
        strcpy(Mqtt_Port     , request->getParam("mqtPort")    ->value().c_str() );
        strcpy( Mqtt_outTopic, request->getParam("mqtoutTopic")->value().c_str() );
        strcpy( Mqtt_inTopic, request->getParam("mqtinTopic")->value().c_str() );
        gas_Idx     =          request->arg("gasidx").toInt();
        el_Idx      =          request->arg("elidx").toInt();
        strcpy( Mqtt_Username, request->getParam("mqtUser")    ->value().c_str() );
        strcpy( Mqtt_Password, request->getParam("mqtPas")     ->value().c_str() );
        //strcpy( Mqtt_Clientid, request->getParam("mqtCi")     ->value().c_str() );  
 
        //Mqtt_switchIDX = request->arg("mqidx").toInt(); 
        Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
     }      

     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}
