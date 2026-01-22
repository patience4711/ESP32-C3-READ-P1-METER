/* this program works as follows: first make the enable pin high
 * next make the serial buffer empty if there is data
 * now wait until data becomes available
 * read until the startsign is found (likely the fst byte)
 * read the next bytes until the endsign is found.
 * if success the crc is checked and if oke the values are extracted from the telegram 
 * and send via mosquitto
 */

void meterPoll() {
// To trigger the poll we only have to put P1_ENABLE HIGH. The meter then sends telegrams, we just pick one 
   consoleOut("polling the meter");

   // the rxpin on the meter should be pulled high, we do this with pin gpio5  (P1_ENABLE) 
  digitalWrite(P1_ENABLE, HIGH); 
   if( read_into_array() ) 
   {
      //we have a telegram
      ledblink(3,300);
      digitalWrite(P1_ENABLE, LOW);
      if(bootTest) 
      {
        // if we are testing we only want to know whether ther is a legegram received
        // so we do not decode it but return  
        if(strlen(teleGram) > 50)
        {
           strcat(teleGram, "\n this is a test telegram, retrieved at boot");
           testFilesave();
           consoleOut("saved a test telegram");
        } else {
           strcpy(teleGram, "test at boot failed");
           consoleOut("test telegram failed");
        }

        consoleOut("performed the bootTest");
        return;
      }
      // when we are not testing we decode the telegram
      decodeTelegram(); 
      sendMqtt(false);
      sendMqtt(true);
    } else {
    //when we are here nothing was received
     consoleOut("no telegram received");
    //if(testPoll) strcpy(teleGram, "testPoll at boot failed");
    //return;
    }

    consoleOut("meterPoll done");
   digitalWrite(P1_ENABLE, LOW);
}

bool read_into_array() {
    int byteCounter = 0;
    char rep[20]={0};
    char inByte[2];
    int Bytes=0;
    polled = false;
    // start waiting until serial1 available   
    waitSerial1Available(5);
    // flush the serial1 buffer so that we start reading at the beginning of the telegram    
    empty_Serial1();
    // now we wait again until something is avialable
    if ( waitSerial1Available(5) ) {

        memset(teleGram, 0, sizeof(teleGram));
        delayMicroseconds(250);
        Serial.println("available on Serial1 : " + String(Serial1.available()));
        while (Serial1.available())
        {
              Serial1.readBytes(inByte, 1);
              // we keep reading until we find the startsign "/"
//              if(diagNose){
//                  if(byteCounter < 7) { 
//                       Serial.println("print the 1st 7 bytes");
//                       Serial.print(inByte[0]);
//                  }
//              }
              byteCounter ++;
              if (inByte[0] == '/') { 
                     consoleOut("\nfound start at " + String(byteCounter));
                    
                    // add this byte to teleGram 
                    strncat(teleGram, inByte, 1); 
                    // now we add the next 650 bytes to teleGram 
                    // until we encounter the endsign
                    for ( int x=0; x < 650; x++) {
                       Serial1.readBytes(inByte, 1);
                       strncat( teleGram, inByte, 1);
                       // catch the endsign
                       if (inByte[0] == '!' ) {
                            consoleOut("found the end sign");

                           // we need to read 4 more bytes (the crc) until the \n and then stop
                           Serial1.readBytes(readCRC, 4);
                           strcat(teleGram, readCRC);
                           polled = true;
                           return true;
                        }   
                     }
               // if we are here, we have read 650 characters but no endsign found
                consoleOut("no endsign found");
               return false;              
           }

       // we terminate if more than 2000 bytes read
       if ( byteCounter > 2000 ) {
             consoleOut("byteCounter over 2000");
            return false;       
           }
        }
   // if we are here, no startssign was found    
       consoleOut("no startsign found");
      return false;
   }

  
  // if we are here, no serial data was available
   consoleOut("got no data from serial1");
  return false;
}   

void decodeTelegram() {
/* this function processes the read telegram
 *  first the crc is calculated and compared
 *  if crc is approved we extract the data from the telegram
 */
      if (polled) {
        //we have a valid telegram, now we can decode it.
        //if no testfile, we write that first
        // before the teleGram is modyfied
//        if( !SPIFFS.exists("/testFile.txt")) {
//            testFilesave(); // an existing file is not overwritten
//        }
         int lengte = strlen(teleGram);
          consoleOut("teleGram length = " + String(lengte));
         
         // the crc = calculated over the telegram inc start and endsign, so without crc
         // the teleGram contains the CRC so we terminate teleGram after the !
         teleGram[lengte - 4] = '\0';
         // now the teleGram is useless for testdecode so:
         //testTelegram = false;
         int calculatedCRC = CRC16(0x0000, (unsigned char *) teleGram, lengte-4); 
         
          consoleOut("the calculated crc = " + String(calculatedCRC));
      
          consoleOut("strol of readCRC = " + String(strtol(readCRC, NULL, 16))); //8F46
    
        if(strtol(readCRC, NULL, 16) == calculatedCRC) //do the crc's match
        {
             consoleOut("crc is correct, now extract values..");
            extractTelegram();   
            polled = true;
            eventSend(2); // inform the webbpage that there is new data
            consoleOut("polled true");
            // set the timestamp
            sprintf( timeStamp, "%02d/%02d %02d:%02d", day(), month(), hour(), minute() );
            return;
        } else {
             consoleOut("crc is wrong, not processed..");
            //failCount++;
            polled=false;
             consoleOut("not polled");
            return;
        }
    }
    
}


void extractTelegram() {
/*
This function extracts the interesting values from the telegram as floats
therefor we call the function returnFloat
with arguments :len ( the line length excl *kWh
: the start of the number and : the length of the number
*/    
char what[24];
      // the meter type in the 1st row /Ene5\XS210 ESMR 5.0
      // find 1-3:0.2.8(50) len = 13 start = 10 count = 2
      strcpy(what, "1-3:0.2.8(");
      if(strstr(teleGram, what )) 
      {
          SMR = round0(returnFloat(what, 13, 10, 2));
           consoleOut("SMR = " + String(SMR));
      }

    // find 1-0:1.8.1(000051.775*kWh) len = 20
      strcpy(what, "1-0:1.8.1(");
      if(strstr(teleGram, what )) {
          CON_LT = returnFloat(what, 20, 10, 10);
           consoleOut("extracted CON_LT = " + String(CON_LT, 3));
      }  
    // find 1-0:1.8.2(000000.000*kWh) len = 20
      strcpy(what, "1-0:1.8.2(");
      if(strstr(teleGram, what )) {
          CON_HT = returnFloat(what, 20, 10, 10);
           consoleOut("extracted CON_HT = " + String(CON_HT, 3));
     }
    // find 1-0:2.8.1(0000524.413*kWh) len = 20
      strcpy(what, "1-0:2.8.1(");
      if(strstr(teleGram, what )) {
          RET_LT = returnFloat(what, 20, 10, 10);
           consoleOut("extracted RET_LT = " + String(RET_LT, 3));
    }  
    // find 1-0:2.8.2(000000.000*kWh) len = 20
      strcpy(what, "1-0:2.8.2(");
      if(strstr(teleGram, what )) {
          RET_HT = returnFloat(what, 20, 10, 10);
           consoleOut("extracted RET_HT = " + String(RET_HT, 3));
    }
    // find 1-0:1.7.0(00.335*kW) len=16 start 10 count 6
      strcpy(what, "1-0:1.7.0(");
      if(strstr(teleGram, what )) {
          POWER_CON[0] = returnFloat(what, 16, 10, 6) * 1000; // watts
           consoleOut("extracted POWER_CON = " + String(POWER_CON[0], 3));
     } 
    // find 1-0:2.7.0(00.000*kW) len=16 start 10 count 6
      strcpy(what, "1-0:2.7.0(");
      if(strstr(teleGram, what )) {
          POWER_RET[0] = returnFloat(what, 16, 10, 6) * 1000; //watts
           consoleOut ("extracted POWER_RET = " + String(POWER_RET[0], 3));         
      }
       
    
    // find 0-1:24.2.1(171105201000W)(00016.713*m3) len 39 start 26 count 9
      strcpy(what, "0-1:24.2.1");
      if(strstr(teleGram, what )) {
          mGAS = returnFloat(what, 39, 26, 9);
           consoleOut ("extracted mGAS = " + String(mGAS, 3));        
      }
}


float returnFloat(char what[24], uint8_t len, uint8_t bgn, uint8_t count) { 
   char extract[len+1];
   char number[16];
   strncpy(extract, strstr(teleGram, what), len);
   // now we have an array starting with the line that contains 'what'
   // Serial.println("extract = " + String(extract));
   // we copy the characters representing the value in tail
   strncpy(number, extract + bgn, count);
   //  Serial.println("tail= " + String(tail));
   // now we have the number, convert it to a float
   return atof(number);
}


