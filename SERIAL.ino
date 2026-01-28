void handle_Serial () {
    Serial.println("\n* * * handle_Serial, type LIST-COMMANDS");  
    int SerialInByteCounter = 0;
    char InputBuffer_Serial[256] = "";
    byte SerialInByte;  
    // first check if there is enough data, at least 13 bytes
    delay(200); //we wait a little for more data as the esp seems slow
    if(Serial.available() < 13 ) {
      // less then 13, we can't expect more so we give up 
      while(Serial.available()) { Serial.read(); } // make the buffer empty 
      Serial.println("invalid command, abort " + String(InputBuffer_Serial));
     return;
    }

// now we know there are at least 13 bytes so we read them
 diagNose = 2; //direct output to serial
 while(Serial.available()) {
             SerialInByte=Serial.read(); 
             //Serial.print("+");
            
            if(isprint(SerialInByte)) {
              if(SerialInByteCounter<130) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            if(SerialInByte=='\n') {                                              // new line character
              InputBuffer_Serial[SerialInByteCounter]=0;
              //   Serial.println(F("found new line"));
             break; // serieel data is complete
            }
       }   
   Serial.println("InputBuffer_Serial = " + String(InputBuffer_Serial) );
   int len = strlen(InputBuffer_Serial);
   // evaluate the incomming data

     //if (strlen(InputBuffer_Serial) > 6) {                                // need to see minimal 8 characters on the serial port
     //  if (strncmp (InputBuffer_Serial,"10;",3) == 0) {                 // Command from Master to RFLink
  
          if (strcasecmp(InputBuffer_Serial, "LIST-COMMANDS") == 0) {
              Serial.println(F("*** AVAILABLE COMMANDS ***"));
              Serial.println(F("DEVICE-REBOOT;"));
              Serial.println(F("PRINTOUT-SPIFFS"));
              Serial.println(F("METERPOLL-TEST;"));
              Serial.println(F("DELETE-FILE=filename; (delete a file)")); 
              Serial.println(F("WRITE-MONTH=; (edit monthly values)"));  
              return;
    } else 

   //8 41.40	25.51	15.98	25.90	5.89
  //9 41.06	30.33	9.97	1644.36	38.48	2.74	4.62	10.64.13	6.10
  // 10 44.36	38.48	2.74	4.62	10.64
  // 11 48.30	47.99	0.77	1.25	50.94
  // 12	94.45	101.43	0.42	0.17	155.28
   // 

   
   if (strncasecmp(InputBuffer_Serial, "WRITE-MONTH=" , 12) == 0) 
   { 
        // the buffer could be something like WRITE-MONTH={"CON_LT":12.34, "CON_HT":56.567, etc}
        // Pointer to JSON start (after '=')
        char *json = strchr(InputBuffer_Serial, '=');
        if (!json) {
            Serial.println("No '=' found");
            return;
          }
        json++;  // move past '='
          
          JsonDocument doc;
          DeserializationError error = deserializeJson(doc, json);

          if (error) {
            Serial.print("JSON error: ");
            Serial.println(error.f_str());
            return;
          }
      
          int month = doc["month"];
         
          MVALS[month].EC_LT = doc["CON_LT"];
          MVALS[month].EC_HT = doc["CON_HT"];
          MVALS[month].ER_LT = doc["RET_LT"];
          MVALS[month].ER_HT = doc["RET_HT"];
          MVALS[month].mGAS =  doc["GAS"];
      
            // write this in SPIFFS
          String bestand = "//mvalues_" + String(month) + ".str"; // month5.str
          writeStruct(bestand, month);
          Serial.println("confirm content " + bestand);
          printStruct( bestand, month ); 
          // // setup a struct for the values of each month
          // typedef struct{
          //   float EC_LT = 0;
          //   float EC_HT = 0;
          //   float ER_LT = 0;
          //   float ER_HT = 0;
          //   float mGAS  = 0;
          // } m_values; 
          // m_values MVALS[13]; 
       return;
    } else   
   
   
    if (strcasecmp(InputBuffer_Serial, "METERPOLL-TEST") == 0) {  
        actionFlag=126;
        return;
    } else

    if (strcasecmp(InputBuffer_Serial, "DEVICE-REBOOT") == 0) {
           Serial.println("\ngoing to reboot ! \n");
           delay(1000);
           ESP.restart();
    } else

    if (strcasecmp(InputBuffer_Serial, "PRINTOUT-SPIFFS") == 0) {
          actionFlag=46;
          return;
    } else
         
    if (strncasecmp(InputBuffer_Serial, "DELETE-FILE=", 12 ) == 0) {  
       Serial.println("len = " + String(len));
       String bestand="";
       for(int i=12;  i<len+1; i++) { bestand += String(InputBuffer_Serial[i]); }
       Serial.println("bestand  = " + bestand);
         // now should have like /bestand.json or so;
         if (SPIFFS.exists(bestand)) 
         {
            Serial.println("going to delete file " + bestand); 
            SPIFFS.remove(bestand);
            Serial.println("file " + bestand + " removed!"); 
         } else { Serial.println("unkown file, forgot the / ??"); }
          return;           
   
      } else {
          
      Serial.println( String(InputBuffer_Serial) + " INVALID COMMAND" );     
     }

       
    // } // end if if (strncmp (InputBuffer_Serial,"10;",3) == 0)
    Serial.println( String(InputBuffer_Serial) + " UNKNOWN COMMAND" );
    //  end if strlen(InputBuffer_Serial) > 6
  // the buffercontent is not making sense so we empty the buffer
  empty_serial();
   //
}   
