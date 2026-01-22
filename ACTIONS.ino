// all actions called by the webinterface should run outside the async webserver environment
// otherwise crashes will occure.
    void test_actionFlag() {
  // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 || value == 11) { // 
     //DebugPrintln("erasing the wifi credentials, value = " + String(value) + "  actionFlag = " + String(actionFlag));
     delay(1000); //reserve time to release the button
     flush_wifi();
     WiFi.disconnect(true);
     //Serial.println(F("wifi disconnected"));
     ESP.restart();
  }  

    if (actionFlag == 10) { // reboot
     delay(2000); // give the server the time to send the confirm
     //DebugPrintln("rebooting");
    // write_eeprom();
     ESP.restart();
  }
    // ********* reconnect mosquitto 
    if (actionFlag == 24) {
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect() ;
       //reset the mqttserver
        MQTT_Client.setServer ( Mqtt_Broker, atoi(Mqtt_Port) );
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
    }    

    if (actionFlag == 23) {
      actionFlag = 0; //reset the actionflag
      empty_serial();
      unsigned long nu = millis();
      consoleOut("nu before = " + String(nu) );
      waitSerial1Available(5); // recalculate time after change of settings
      nu = millis();
      consoleOut("nu after = " + String(nu) );
      if(Serial.available())  consoleOut("there is data available");
    }    
    

    if (actionFlag == 26) { //polling
      actionFlag = 0; //reset the actionflag
      meterPoll(); // read and decode the telegram
      sendMqtt(false);
      sendMqtt(true); //gas
    }    
    if (actionFlag == 126) { //polling
      actionFlag = 0; //reset the actionflag
      consoleOut("running read_test");
      read_test(); // read 100 chars in teleGram
    }

    // decode the testfile
    if (actionFlag == 28) { //
      actionFlag = 0; //reset the actionflag
      if(!testTelegram) {
        consoleOut("telegram is modified, reboot if you want to test");
        return;
      }
      // if we have te telegram read from spiffs we can decode it
      polled=true;
      //we need the readCRC so we extract it from the file
         int len = strlen(teleGram);
         strncpy(readCRC, teleGram + len-4, 4); 
         consoleOut("readCRC = " + String(readCRC) );
         decodeTelegram();
         sendMqtt(false);
         sendMqtt(true);
    }

// test the reception of a telegram at boot
// the teleGram is saved in SPIFFS

    if (actionFlag == 30) { 
      actionFlag = 0; //reset the actionflag
      //if (SPIFFS.exists("/testFile.txt")) SPIFFS.remove("/testFile.txt");      
      read_into_array(); // 
      // save the outcome to the files
      testFilesave(); // an existing file is not overwritten

    }    

    if (actionFlag == 46) { //triggered by the console printout-spiffs
        consoleOut("print the files");
        actionFlag = 0; //reset the actionflag
        showDir();
        printFiles(); 
    }

     if (actionFlag == 48) { //triggered by console test-Serial1   
     actionFlag = 0;
     testMessage();
     }
    // ** test mosquitto *********************
    if (actionFlag == 49) { //triggered by console testmqtt
        actionFlag = 0; //reset the actionflag
        ledblink(1,100);
        // always first drop the existing connection
        MQTT_Client.disconnect();
        ws.textAll("dropped connection");
        delay(100);
        char Mqtt_send[26] = {0};
       
        if(mqttConnect() ) {
        String toMQTT=""; // if we are connected we do this
        
        strcpy( Mqtt_send , Mqtt_outTopic);
        
        //if(Mqtt_send[strlen(Mqtt_send -1)] == '/') strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
        toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
        
        if(Mqtt_Format == 5) toMQTT = "field1=12.3&field4=44.4&status=MQTTPUBLISH";
        
        if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
            consoleOut("sent mqtt message : " + toMQTT);
        } else {
            consoleOut("sending mqtt message failed : " + toMQTT);    
        }
      } 
     // the not connected message is displayed by mqttConnect()
    }


} // end test actionflag
