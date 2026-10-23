
/* this program works as follows: first make the enable pin high
 * next make the serial buffer empty if there is data
 * now wait until data becomes available
 * read until the startsign is found (likely the fst byte)
 * read the next bytes until the endsign is found.
 * if success the crc is checked and if oke the values are extracted from the telegram 
 * and send via mosquitto
 */

void meterPoll(bool fastPoll) {
#ifdef TESTCONFIG
// do a fakePoll
consoleOut("doing a fakePoll");
   fakePoll(fastPoll);
   if(! fastPoll) {
     // we did a slow poll so we need to send an event
     consoleOut("complete fakePoll");
     eventSend(2);
   } else {
     consoleOut("fast fakePoll");
   }
   consoleOut("templateMade = " + String(templateMade));
   polled=true;
   sprintf( timeStamp, "%02d/%02d %02d:%02d:%02d", day(), month(), hour(), minute(), second() );
   return;
#endif       
// To trigger the poll we only have to put P1_ENABLE HIGH. The meter then sends telegrams, we just pick one 
   unsigned long startPolling = millis();

   // the rxpin on the meter should be pulled high, we do this with pin gpio5  (P1_ENABLE) 
  digitalWrite(P1_ENABLE, HIGH); 
   if( read_into_array() ) 
   {
      //we have a telegram and can analyse it
      //if there is no telegram, no ledsignal
      if(ledSignal) my_wifiConnect.ledOn(50,0,0);
      digitalWrite(P1_ENABLE, LOW);
      polled = true;
      //we decode the telegram
      decodeTelegram(fastPoll); 
      sprintf( timeStamp, "%02d/%02d %02d:%02d:%02d", day(), month(), hour(), minute(), second() );
      if(Mqtt_Format != 0) {
         sendMqtt(false);
         sendMqtt(true);
      }
      if(fastPoll == false) {
        makeJsonTemplateHW();
        makeJsonTemplateWU();
        eventSend(2);
         consoleOut("did a complete Poll * * * * * * * * * * * *");
      } else {
         consoleOut("did a fastPoll"); 
      }  //fastPoll = true;
      
      pollDurationMs = millis() - startPolling;
      //pollCounter ++;
    } else {
    //when we are here nothing was received
     consoleOut("no telegram received");
    }
     if(ledSignal) my_wifiConnect.ledOff();
     consoleOut("meterPoll done");
     digitalWrite(P1_ENABLE, LOW);
}



// werkende versie
bool read_into_array() 
{
 unsigned long startReading = millis();   
    int byteCounter = 0;
    //int maxTry = 1;
    polled = false;
    //Serial1.setTimeout
    //if (fastPoll == false) maxTry = 5;
    
    // Start wachten tot Serial1 beschikbaar is
    //waitSerial1Available(3);
    //empty_Serial1(); // Flush de oude buffer
    
    if (waitSerial1Available(3)) 
    {
        memset(teleGram, 0, sizeof(teleGram));
        delayMicroseconds(250);
        
        unsigned long startTijdLus = millis();
        bool startGevonden = false;
        int x = 0;

        // Oneindige lus totdat het telegram compleet is of we een timeout krijgen (max 150ms)

        while (millis() - startTijdLus < 150) 
        {
            if (Serial1.available() > 0) 
            {
                char c = Serial1.read();
                byteCounter++;

                if (!startGevonden) {
                    if (c == '/') {
                        startGevonden = true;
                        teleGram[0] = c;
                        //teleGram[1] = '\0';
                        x = 1;
                        consoleOut("\nfound start at " + String(byteCounter));
                    }
                } 
                else // We zijn het telegram aan het vullen
                {
                    if (x < 1499) {
                        teleGram[x] = c;
                        //teleGram[x + 1] = '\0';
                        x++;
                    }

                    // Catch the endsign
                    if (c == '!') {
                        consoleOut("found the end sign");
                        
                        // Lees direct de 4 CRC bytes die er direct achteraan rollen
                        int crcIdx = 0;
                        unsigned long crcWait = millis();
                        // Heel even kort wachten tot de allerlaatste 4 bytes binnenvallen
                        while(Serial1.available() < 4 && (millis() - crcWait < 20)) {  }
                        
                        while (Serial1.available() > 0 && crcIdx < 4) {
                            readCRC[crcIdx] = Serial1.read();
                            crcIdx++;
                        }
                        readCRC[crcIdx] = '\0';
                        
                        strcat(teleGram, readCRC);
                        polled = true;
                        readDurationMs = millis() - startReading;
                        return true;
                    }
                
                    if (byteCounter > 2000) {
                        consoleOut("byteCounter over 2000");
                        return false;
                    }
                }
            }
        }
    }
    consoleOut("got no data from serial1");
    return false;
}




bool waitSerial1Available(int howMany) // wait untill something 's available
{
if(ledSignal) my_wifiConnect.ledBlink(255,0,0,howMany, 30);
consoleOut("waitSerial1Available");
    for(int y=0; y < howMany; y++) {
     if(ledSignal) my_wifiConnect.ledBlink(255,0,0,2, 20);        
     consoleOut("y = " + String(y) );
      unsigned long wait = millis();
      while ( !Serial1.available() )
          {
          if ( millis() - wait > 2000) break; // after 2000 milis time out
          }
          // when timed out we check
      if ( Serial1.available() ) return true;
           
    }
  return false;
}

void decodeTelegram(bool fast) {
/* this function processes the read telegram
 *  first the crc is calculated and compared
 *  if crc is approved we extract the data from the telegram
 */
      if (polled) {
        //unsigned long startDecode = millis();
        //we have a valid telegram, now we can decode it.

         int lengte = strlen(teleGram);
          consoleOut("teleGram length = " + String(lengte));
         
         // the crc = calculated over the telegram inc start and endsign, so without crc
         // the teleGram contains the CRC so we terminate teleGram after the !
         teleGram[lengte - 4] = '\0';

         // this part is used to do the crc check, we can skip that when we are in the fast modus
         if(fast == false)
         {
            int calculatedCRC = CRC16(0x0000, (unsigned char *) teleGram, lengte-4); 
            
              consoleOut("the calculated crc = " + String(calculatedCRC));
          
              consoleOut("strol of readCRC = " + String(strtol(readCRC, NULL, 16))); //8F46
        
            if(strtol(readCRC, NULL, 16) == calculatedCRC) //do the crc's match
            {
                consoleOut("crc is correct, now extract values..");
                parseTelegram(fast);   
                polled = true;
                
                consoleOut("polled true");
                //decodeDurationMs = millis() - startDecode;
                return;
            } else {
                consoleOut("crc is wrong, not processed..");
                //failCount++;
                polled=false;
                consoleOut("not polled");
                return;
            }
         } else {
                // do this without crc
                parseTelegram(fast);   
                polled = true;
                //eventSend(2); // inform the webbpage that there is new data
                consoleOut("polled true");
                //decodeDurationMs = millis() - startDecode;
                return;
         }  
    }
    
}
