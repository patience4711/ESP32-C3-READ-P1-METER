void handleForms(AsyncWebServerRequest *request) {
     //every form submission is handled here basisconfig geoconfig mqtt-config
     // we find out which form with a parameter present 
     String serverUrl = request->url().c_str();
     Serial.println("serverUrl = " + serverUrl); // this is /submitform 

     if(request->hasParam("mType")) {
         // received form basisconfig
         strcpy(userPwd, request->arg("pw1").c_str());
         // the metertype is a char
         String pValue = request->getParam("mType")->value();
         // copy safe to the char-array (max 29 characters + \0 terminator)
         memset(meterType, 0, sizeof(meterType)); // first empty the array 
         pValue.toCharArray(meterType, sizeof(meterType));
              
         pollFreq = request->arg("pfreq").toInt(); //values are 3 4 5 6 7 30 60 300
         //if(request->hasParam("pL")) Polling = true; else Polling = false;
         if(request->hasParam("debug")) diagNose = true;  else diagNose = false;
         if(request->hasParam("lsig")) ledSignal = true;  else ledSignal = false;
         if(request->hasParam("brt")) baud9600 = true;  else baud9600 = false;
         if(request->hasParam("rxi")) rxInvert = true;  else rxInvert = false;

                // received form basisconfig
          if(request->hasParam("3ph")) threePhase = true;  else threePhase = false;
         basisConfigsave();  // alles opslaan
         if(pollFreq < 10 ) {
            Mqtt_Format = 0;
            mqttConfigsave();
         }
         return; 
     } else 

      if(request->hasParam("longi")) {
        // received the geoconfig form
        longi = request->getParam("longi")->value().toFloat();
        lati = request->getParam("be")->value().toFloat();
        strcpy(gmtOffset, request->getParam("tz")->value().c_str());
        // a checkbox has only a parameter when checked so we check only "is there a param"
        if(request->hasParam("ts")) zomerTijd = true;  else  zomerTijd = false;
        wifiConfigsave();
        actionFlag=25; // recalculate with these settings 
        return;
     } else
    
      if(request->hasParam("econht")) {
        // received the setvalues form  names are econht, econlt eretht, eretlt, mnr
        int mth = request->getParam("mnr")->value().toInt();
        
        MVALS[mth].EC_HT = request->getParam("econht")->value().toFloat();
        MVALS[mth].EC_LT = request->getParam("econlt")->value().toFloat();
        MVALS[mth].ER_HT = request->getParam("eretht")->value().toFloat();
        MVALS[mth].ER_LT = request->getParam("eretlt")->value().toFloat();
        MVALS[mth].mGAS = request->getParam("mGas")->value().toFloat();
        String bestand = "/mvalues_" + String(mth) + ".str"; // month5.str
        writeStruct(bestand, mth);
        return;
     } else
     
     
     
     if(request->hasParam("mqtAdres")) {
        // form mosquitto received
        strcpy( Mqtt_Broker  , request->getParam("mqtAdres")   ->value().c_str() );
        Mqtt_Port = request->arg("mqtPort").toInt();
        strcpy( Mqtt_outTopic, request->getParam("mqtoutTopic")->value().c_str() );
        gas_Idx     =          request->arg("gasidx").toInt();
        el_Idx      =          request->arg("elidx").toInt();
        strcpy( Mqtt_Username, request->getParam("mqtUser")    ->value().c_str() );
        strcpy( Mqtt_Password, request->getParam("mqtPas")     ->value().c_str() );
        strcpy( Mqtt_Clientid, request->getParam("mqtCi")     ->value().c_str() );  
 
        //Mqtt_switchIDX = request->arg("mqidx").toInt(); 
        Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
     }      

     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}
