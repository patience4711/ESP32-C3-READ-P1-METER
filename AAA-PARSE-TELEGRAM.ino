/*
#define OBIS_SMR             "1-3:0.2.8("
#define define OBIS_CON_LT   "1-0:1.8.1("
#define define OBIS_CON_HT   "1-0:1.8.2("
#define define OBIS_RET_LT   "1-0:2.8.1("
#define define OBIS_RET_HT   "1-0:2.8.2(" 

#define define OBIS_POWER_C1 "1-0:21.7.0("
#define define OBIS_POWER_R1 "1-0:22.7.0("
#define define OBIS_POWER_C2 "1-0:41.7.0("
#define define OBIS_POWER_R2 "1-0:42.7.0("
#define define OBIS_POWER_C3 "1-0:61.7.0("
#define define OBIS_POWER_R3 "1-0:62.7.0("

#define define OBIS_GAS      "0-1:24.2.1("
*/


void parseTelegram()
{
    const char* p = teleGram;
    consoleOut("parsing teleGram");
    while (*p)
    {
        // DSMR version
        if (!strncmp(p, OBIS_SMR, strlen(OBIS_SMR)))  meter.smr = (uint8_t)parseValue(p, OBIS_SMR);
           //consoleOut("meter.smr = " + String(meter.smr));
        // Energy consumed
        else if (!strncmp(p, OBIS_CON_LT, strlen(OBIS_CON_LT))) meter.con_lt = parseValue(p, OBIS_CON_LT);
        
        else if (!strncmp(p, OBIS_CON_HT, strlen(OBIS_CON_HT))) meter.con_ht = parseValue(p, OBIS_CON_HT);

        // Energy returned
        else if (!strncmp(p, OBIS_RET_LT, strlen(OBIS_RET_LT)))  meter.ret_lt = parseValue(p, OBIS_RET_LT);

        else if (!strncmp(p, OBIS_RET_HT, strlen(OBIS_RET_HT)))  meter.ret_ht = parseValue(p, OBIS_RET_HT);

        //  power per phase
        else if (!strncmp(p, OBIS_POWER_C1, strlen(OBIS_POWER_C1))) meter.pwr_con[0] = (uint16_t)(parseValue(p, OBIS_POWER_C1) * 1000.0f); 
        
        else if (!strncmp(p, OBIS_POWER_R1, strlen(OBIS_POWER_R1)))  meter.pwr_ret[0] = (uint16_t)(parseValue(p, OBIS_POWER_R1) * 1000.0f);

        else if (!strncmp(p, OBIS_POWER_C2, strlen(OBIS_POWER_C2)))  meter.pwr_con[1] = (uint16_t)(parseValue(p, OBIS_POWER_C2) * 1000.0f); 

        else if (!strncmp(p, OBIS_POWER_R2, strlen(OBIS_POWER_R2)))  meter.pwr_ret[1] = (uint16_t)(parseValue(p, OBIS_POWER_R2) * 1000.0f);

        else if (!strncmp(p, OBIS_POWER_C3, strlen(OBIS_POWER_C3)))  meter.pwr_con[2] = (uint16_t)(parseValue(p, OBIS_POWER_C3) * 1000.0f); 

        else if (!strncmp(p, OBIS_POWER_R3, strlen(OBIS_POWER_R3)))  meter.pwr_ret[2] = (uint16_t)(parseValue(p, OBIS_POWER_R3) * 1000.0f); 

        else if (!strncmp(p, OBIS_GAS, strlen(OBIS_GAS))) meter.gas = parseGasValue(p);                              

        // move to next line
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++; // now we are on the next line
    }
}

// helper function
float parseValue(const char* p, const char* obis)
{
    return atof(p + strlen(obis));
}

float parseGasValue(const char* p)
{
    // find first '('
    p = strchr(p, '(');
    if (!p) return NAN;

    // find second '('
    p = strchr(p + 1, '(');
    if (!p) return NAN;

    return atof(p + 1);
}

// helper function
// float parseValue(const char* p)
// {
//     p = strchr(p, '(');
//     if (!p) return NAN;
//     return atof(p + 1);
// }



// void extractTelegram() {
// /*
// This function extracts the interesting values from the telegram as floats
// therefor we call the function returnFloat
// with arguments :len ( the line length excl *kWh
// : the start of the number and : the length of the number
// */    
// char what[24];
//       // the meter type in the 1st row /Ene5\XS210 ESMR 5.0
//       // find 1-3:0.2.8(50) len = 13 start = 10 count = 2
//       strcpy(what, "1-3:0.2.8(");
//       if(strstr(teleGram, what )) 
//       {
//           SMR = round0(returnFloat(what, 13, 10, 2));
//            consoleOut("SMR = " + String(SMR));
//       }

//     // find 1-0:1.8.1(000051.775*kWh) len = 20
//       strcpy(what, "1-0:1.8.1(");
//       if(strstr(teleGram, what )) {
//           CON_LT = returnFloat(what, 20, 10, 10);
//            consoleOut("extracted CON_LT = " + String(CON_LT, 3));
//       }  
//     // find 1-0:1.8.2(000000.000*kWh) len = 20
//       strcpy(what, "1-0:1.8.2(");
//       if(strstr(teleGram, what )) {
//           CON_HT = returnFloat(what, 20, 10, 10);
//            consoleOut("extracted CON_HT = " + String(CON_HT, 3));
//      }
//     // find 1-0:2.8.1(0000524.413*kWh) len = 20
//       strcpy(what, "1-0:2.8.1(");
//       if(strstr(teleGram, what )) {
//           RET_LT = returnFloat(what, 20, 10, 10);
//            consoleOut("extracted RET_LT = " + String(RET_LT, 3));
//     }  
//     // find 1-0:2.8.2(000000.000*kWh) len = 20
//       strcpy(what, "1-0:2.8.2(");
//       if(strstr(teleGram, what )) {
//           RET_HT = returnFloat(what, 20, 10, 10);
//            consoleOut("extracted RET_HT = " + String(RET_HT, 3));
//     }
//     // find 1-0:1.7.0(00.335*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:21.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_CON[0] = returnFloat(what, 16, 10, 6) * 1000; // watts
//            consoleOut("extracted POWER_CON = " + String(POWER_CON[0], 3));
//      } 
//     // find 1-0:2.7.0(00.000*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:22.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_RET[0] = returnFloat(what, 16, 10, 6) * 1000; //watts
//            consoleOut ("extracted POWER_RET = " + String(POWER_RET[0], 3));         
//       }
//      // find 1-0:1.7.0(00.335*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:41.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_CON[0] = returnFloat(what, 16, 10, 6) * 1000; // watts
//            consoleOut("extracted POWER_CON = " + String(POWER_CON[1], 3));
//      } 
//     // find 1-0:2.7.0(00.000*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:42.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_RET[0] = returnFloat(what, 16, 10, 6) * 1000; //watts
//            consoleOut ("extracted POWER_RET = " + String(POWER_RET[1], 3));         
//       }      
//     // find 1-0:1.7.0(00.335*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:61.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_CON[0] = returnFloat(what, 16, 10, 6) * 1000; // watts
//            consoleOut("extracted POWER_CON = " + String(POWER_CON[2], 3));
//      } 
//     // find 1-0:2.7.0(00.000*kW) len=16 start 10 count 6
//       strcpy(what, "1-0:62.7.0(");
//       if(strstr(teleGram, what )) {
//           POWER_RET[0] = returnFloat(what, 16, 10, 6) * 1000; //watts
//            consoleOut ("extracted POWER_RET = " + String(POWER_RET[2], 3));         
//       }    
//     // find 0-1:24.2.1(171105201000W)(00016.713*m3) len 39 start 26 count 9
//       strcpy(what, "0-1:24.2.1");
//       if(strstr(teleGram, what )) {
//           mGAS = returnFloat(what, 39, 26, 9);
//            consoleOut ("extracted mGAS = " + String(mGAS, 3));        
//       }
// }


// void parseTelegram() {
// /*
// OBIS is 
// This function extracts the interesting values from the telegram as floats
// therefor we call the function getObisValue
// with arguments the obiscode+(  e.g. "1-3:0.2.8("
// */    
//       // the meter type in the 1st row /Ene5\XS210 ESMR 5.0
//       // find 1-3:0.2.8
//       SMR = round(getObisValue(OBIS_SMR));
//       consoleOut("SMR = " + String(SMR));
      
//     // find 1-0:1.8.1(000051.775*kWh) len = 20
//       CON_LT = getObisValue(OBIS_CON_LT);
//       consoleOut("extracted CON_LT = " + String(CON_LT, 3));
    
//     // find 1-0:1.8.2(000000.000*kWh) len = 20
//       CON_HT = getObisValue(OBIS_CON_HT);
//       consoleOut("extracted CON_HT = " + String(CON_HT, 3));

//     // find 1-0:2.8.1(0000524.413*kWh) len = 20
//       RET_LT = getObisValue(OBIS_RET_LT);
//       consoleOut("extracted RET_LT = " + String(RET_LT, 3));
  
//     // find 1-0:2.8.2(000000.000*kWh) len = 20
//       RET_HT = getObisValue(OBIS_RET_HT);
//       consoleOut("extracted RET_HT = " + String(RET_HT, 3));    

//     // find 1-0:21.7.0(00.335*kW) len=16 start 10 count 6
//       POWER_CON[0] = getObisValue(OBIS_POWER_C1);
//       consoleOut("extracted POWER_CON[0] = " + String(POWER_CON[0], 3)); 

//     // find 1-0:22.7.0(00.335*kW) len=16 start 10 count 6
//       POWER_RET[0] = getObisValue(OBIS_POWER_R1);
//       consoleOut("extracted POWER_RET[0] = " + String(POWER_RET[0], 3));     
 
//     // find 1-0:21.7.0(00.335*kW) len=16 start 10 count 6
//       POWER_CON[1] = getObisValue(OBIS_POWER_C2);
//       consoleOut("extracted POWER_CON[1] = " + String(POWER_CON[1], 3)); 

//       // find 1-0:22.7.0(00.335*kW) len=16 start 10 count 6
//       POWER_RET[1] = getObisValue(OBIS_POWER_R2);
//       consoleOut("extracted POWER_RET[1] = " + String(POWER_RET[1], 3));          
//       }

//     // find 1-0:61.7.0(00.335*kW) len=16 start 10 count 6
//       POWER_CON[2] = getObisValue(OBIS_POWER_C3);
//       consoleOut("extracted POWER_CON[2] = " + String(POWER_CON[2], 3)); 
    
//     // find 1-0:62.7.0(00.335*kW) len=16 start 10 count 6 
//       POWER_RET[2] = round(getObisValue(OBIS_POWER_R2));
//       consoleOut("extracted POWER_RET[2] = " + String(POWER_RET[2], 3));          
//       }

    
//     // find 0-1:24.2.1(171105201000W)(00016.713*m3) len 39 start 26 count 9
      
//       mGas = getObisValue(OBIS_GAS);
//       consoleOut ("extracted mGAS = " + String(mGAS, 3));    
    

// }




// float returnFloat(char what[24], uint8_t len, uint8_t bgn, uint8_t count) { 
//    char extract[len+1];
//    char number[16];
//    strncpy(extract, strstr(teleGram, what), len);
//    // now we have an array starting with the line that contains 'what'
//    // Serial.println("extract = " + String(extract));
//    // we copy the characters representing the value in tail
//    strncpy(number, extract + bgn, count);
//    //  Serial.println("tail= " + String(tail));
//    // now we have the number, convert it to a float
//    return atof(number);
// }

