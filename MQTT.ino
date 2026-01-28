bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
   
    if( MQTT_Client.connected() ) {
    consoleOut("mqtt was connected");
    return true;
    }
     consoleOut(F("\nconnect mqtt"));
    // we are here because w'r not connected. Signal with the LED
    //ledblink(2,70);

    //if (Mqtt_Port == 0 ) { Mqtt_Port = 1883;}   // just in case ....
    uint8_t retry = 3;
    //String Mqtt_Clientid = "ESP32-C3-P1-" + getChipId(true); // to get a client id
    //char Mqtt_inTopic[11]={"ESP-ECU/in"};

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( Mqtt_Clientid, Mqtt_Username, Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if( Mqtt_Format != 0 ) {
           
           if( MQTT_Client.subscribe ( Mqtt_inTopic ) ) {
               consoleOut("subscribed to " + String(Mqtt_inTopic));
           }
        }
         consoleOut(F("mqtt connected"));
        // UpdateLog(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        //UpdateLog(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.

  consoleOut(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
   
//    String Payload = "";     // convert the payload to a String...
//    for (int i = 0; i < length; i++)
//    {
//        Payload += (char)payload[i]; // convert to char, needed???
//    }
    
   // ws.textAll("mqtt received " + Payload);

    JsonDocument doc;       // We use json library to parse the payload                         
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
   // DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload); // Deserialize the JSON document
    if (error)            // Test if parsing succeeds.
    {
       if(diagNose)  ws.textAll("mqtt no valid json ");
        return;
    } 
    
    // We check the kind of command format received with MQTT
    //now we have a payload like {"poll",1}    

    if( !doc["poll"].isNull() )
    {
        //int inv = doc["poll"].as<int>(); 
         consoleOut( "got message containing \"poll\"");

        if(pollFreq == 0)
        {
             actionFlag = 26; // takes care for the polling
              return;
            } else {
                consoleOut("forbidden, automatic polling is on");
              return;         
            }
        }
        else
        {
           consoleOut("polling = automatic, skipping");
        }
}

void sendMqtt(bool gas) {

if(Mqtt_Format == 0) return;  

  char Mqtt_send[26]={0};  
  strcpy(Mqtt_send, Mqtt_outTopic);
//  if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
//    strcat(Mqtt_send, String(Inv_Prop[which].invIdx).c_str());
//  }
  bool reTain = false;
  char pan[50]={0};
  char tail[40]={0};
  char toMQTT[512]={0};

// the json to p1 domoticz must be something like {"command":"udevice", "idx":1234, "svalue":"lu;hu;lr;hr;ac;ar"}
// the json to gas {"command":"udevice", "idx":1234, "svalue":"3.45"} 
//where lu is low tariff usage Wh, hu is high tariff usage  Wh), lr is low tariff return Wh), 
//hr is high tariff return  Wh, ac is actual power consumption (in W) and ar is actual return W .  
   switch( Mqtt_Format)  { 
    case 1: 
       if(!gas) {
        snprintf(toMQTT, sizeof(toMQTT), "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\"}" , el_Idx, meter.con_lt*1000 , meter.con_ht*1000, meter.ret_lt*1000, meter.ret_ht*1000, meter.pwr_con[0], meter.pwr_ret[0]);
       } else {
        snprintf(toMQTT, sizeof(toMQTT), "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.3f;\"}", gas_Idx, meter.gas);
       }
       break;
    case 2:
       snprintf(toMQTT, sizeof(toMQTT), "{\"econ_lt\":%.2f,\"econ_ht\":%.2f,\"eret_ht\":%.2f,\"eret_lt\":%.2f,\"actualp_con\":%.2f,\"actualp_ret\":%.2f,\"gas\":%.3f}" , meter.con_lt, meter.con_ht, meter.ret_lt, meter.ret_ht, meter.pwr_con[0], meter.pwr_ret[0], meter.gas);
       break;
    case 3:
       snprintf(toMQTT, sizeof(toMQTT), "field1=%.3f&field2=%.3f&field3=%.3f&field4=%.3f&field5=%.0f&field6=%.0f&field7=%.3f&status=MQTTPUBLISH" ,meter.con_lt, meter.con_ht, meter.ret_lt, meter.ret_ht, meter.pwr_con[0], meter.pwr_ret[0], meter.gas);
       reTain=false;
       break;
     }

   // mqttConnect() checks first if we are connected, if not we connect anyway
   if(mqttConnect() ) MQTT_Client.publish ( Mqtt_send, toMQTT, reTain );
}