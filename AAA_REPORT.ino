String putReport(const String& var) 
{
      String resultaat = "";

      if(var == "rows") 
      {
         char report1[1200] = {0}; // Grotere buffer omdat we nu veel meer HTML-tekst genereren
         char report2[1200] = {0}; 
         char temp[250] = {0};     // Grotere temp buffer voor de extra kolommen
         
         // Variabelen voor de berekende maandverbruiken
         float econ_lt, econ_ht, eret_lt, eret_ht, egas;
         int lastMonth;
         int maand = month() + 1;

         for(int x = 1; x < 13; x++) 
         {
               econ_lt = 0; econ_ht = 0; eret_lt = 0; eret_ht = 0; egas = 0;
               
               lastMonth = (x == 1) ? 12 : x - 1;

               if (x == month()) 
               {
                  // HUIDIGE MAAND: Actuele meterstanden minus de stand van de vorige maand in MVALS
                  econ_lt = meter.con_lt - MVALS[lastMonth].EC_LT;
                  econ_ht = meter.con_ht - MVALS[lastMonth].EC_HT;
                  eret_lt = meter.ret_lt - MVALS[lastMonth].ER_LT;
                  eret_ht = meter.ret_ht - MVALS[lastMonth].ER_HT;
                  egas    = meter.gas    - MVALS[lastMonth].mGAS;
               } 
               else 
               {
                  // ALLE ANDERE MAANDEN: Stand van deze maand minus stand van de maand ervoor
                  if (MVALS[x].EC_LT == 0 && MVALS[x].EC_HT == 0 && MVALS[x].ER_LT == 0 && MVALS[x].ER_HT == 0 && MVALS[x].mGAS == 0)
                  {
                        // GEREPAREERD: We gebruiken %s voor de "n/a" teksten
                        sprintf(temp, "<tr id=\"row%d\"><td>%d<td>%s<td>%s<td>%s<td>%s<td>%s</tr>\n", 
                              x, x, "n/a", "n/a", "n/a", "n/a", "n/a"); 
                        
                        if(maand > x) {
                           strcat(report2, temp);
                        } else { 
                           strcat(report1, temp);
                        }
                        
                        continue; // Spring meteen door naar de volgende maand, sla de berekening hieronder over
                  }
                  econ_lt = MVALS[x].EC_LT - MVALS[lastMonth].EC_LT;
                  econ_ht = MVALS[x].EC_HT - MVALS[lastMonth].EC_HT;
                  eret_lt = MVALS[x].ER_LT - MVALS[lastMonth].ER_LT;
                  eret_ht = MVALS[x].ER_HT - MVALS[lastMonth].ER_HT;
                  egas    = MVALS[x].mGAS  - MVALS[lastMonth].mGAS;
               }

               // Bouw de HTML-rij met 6 kolommen (Maand + 5 verbruikswaarden)
               sprintf(temp, 
                       "<tr id=\"row%d\">"
                       "<td>%d</td>"
                       "<td>%.2f</td>"
                       "<td>%.2f</td>"
                       "<td>%.2f</td>"
                       "<td>%.2f</td>"
                       "<td>%.2f</td>"
                       "</tr>\n", 
                       x, x, econ_lt, econ_ht, eret_lt, eret_ht, egas);  
               
               if(maand > x) {
                  strcat(report2, temp);
               } else { 
                  strcat(report1, temp);
               }
         }
         
         strcat(report1, report2);
         //resultaat = String(report1);
         
         //Serial.println(resultaat);
         return report1;
      } 
      
      return String();
}


// String putReport(const String& var)
// {
// //swap_to_usb();
//   if(var == "rows") {
//         char report1[1200] = {0}; //12 x 100
//         char report2[1200] = {0}; //12 x 100
//         char temp[100] = {0}; //4 floats of 9 and one of 8 = 45 and 35 html 
//         float econ_lt;
//         float econ_ht;
//         float eret_lt;
//         float eret_ht;
//         float mgas;
//         int next;
//         int prev;
//         int maand = month()+1;
//         // the monthly values are written at the very beginning of each month so month2 is end month1
//         // if the nextmonth = smaller, this is a value of a former year
//         // we can not calculate it unless x - the current month, thec we can calculate the todate value
        
//         // we have 12 values 1 to 12
//         //for mont 1 the value = 2 -/- 1 and 11 = 12 -/-11 and 12 = 1-12
//         //the struct goes fom 0 to 11
        
//         for(int x=1; x<13; x++) {
       
//         econ_lt = 0;
//         econ_ht = 0;
//         eret_lt = 0;
//         eret_ht = 0;
//         mgas = 0;       
// //        
//         next = x+1;
//         if(next == 13) next = 0; // the last iteration is thus 0 - 11   
// //        prev = x-1;
// //        if(prev == 0) prev = 12; 
//          Serial.println("x = " + String(x));
//         //if there are no values we skip this whole iteration
//         if ( MVALS[x].EC_LT != 0 || MVALS[x].EC_HT != 0 || MVALS[x].ER_LT != 0 || MVALS[x].ER_HT != 0 || MVALS[x].mGAS != 0 )
//         { 
//             Serial.println("found values in month " + String(x));
//             // we are in the month 8 and have values
//             //check if there are values in the next month and they are greater
//             // if we have greater values of the next month, we could calculate
//             if( MVALS[next].EC_LT != 0 &&  MVALS[next].EC_LT > MVALS[x].EC_LT ) { 
//             Serial.println("also values in next month " + String(next));
//                    econ_lt = MVALS[next].EC_LT - MVALS[x].EC_LT;
//                    // this would be over the current month
//             } else {
//             // we have values in x but not (greater) values in next
//             // we can check if the current values are greater than the ones in the file
//             // we can calculate a todate value that would be over the current mont 
//             Serial.println("no values in next month so calculate todate"); 
//                if (meter.con_lt > MVALS[x].EC_LT ) {
//                  econ_lt = meter.con_lt - MVALS[x].EC_LT; //
//                } else { 
//                 Serial.println("could not calculate todate for " + String(x));}
//             }  
          
//             if( MVALS[next].EC_HT != 0 &&  MVALS[next].EC_HT > MVALS[x].EC_HT ) { 
//                econ_ht = MVALS[next].EC_HT - MVALS[x].EC_HT;
//             } else {
//                if (meter.con_ht > MVALS[x].EC_HT ) econ_ht = meter.con_ht - MVALS[x].EC_HT; //
//             }            

//             if( MVALS[next].ER_LT != 0 &&  MVALS[next].ER_LT > MVALS[x].ER_LT ) { 
//                eret_lt = MVALS[next].ER_LT - MVALS[x].ER_LT;
//             } else {
//                if (meter.ret_lt > MVALS[x].ER_LT ) eret_lt = meter.ret_lt - MVALS[x].ER_LT; //
//             } 
            
//             if( MVALS[next].ER_HT != 0 &&  MVALS[next].ER_HT > MVALS[x].ER_HT ) { 
//                eret_ht = MVALS[next].ER_HT - MVALS[x].ER_HT;
//             } else {
//                if (meter.ret_ht > MVALS[x].ER_HT ) eret_ht = meter.ret_ht - MVALS[x].ER_HT; //
//             }             


//             if( MVALS[next].mGAS != 0 &&  MVALS[next].mGAS > MVALS[x].mGAS ) { 
//                mgas = MVALS[next].mGAS - MVALS[x].mGAS;
//             } else {
//                if (meter.gas > MVALS[x].mGAS ) mgas = meter.gas - MVALS[x].mGAS; //
//             }             
          
//        } else { Serial.println("skipped line " + String(x));}    
//         // for each month we print a line
//         // we want the current month always be the last one
//         sprintf(temp, "<tr id=\"row%.d\"><td>%.d<td>%.2f<td>%.2f<td>%.2f<td>%.2f<td>%.2f</td></tr>", next, next, econ_lt, econ_ht, eret_lt, eret_ht, mgas);  
//         if( maand > x) {
//             strcat(report2, temp); // so 9 10 11 12
//         } else { 
//             strcat(report1, temp);} //  1 - 8
//         }    
//         strcat(report1, report2); 
//         //console_Log(String(report));
//         consoleOut("length = " + String(strlen(report2)));
//         //swap_to_hw();
//         return report1;
//         }

// return String();
// }
// String putReport(const String& var)
// {
// //swap_to_usb();
//   if(var == "rows") {
//         char report1[1200] = {0}; //12 x 100
//         char report2[1200] = {0}; //12 x 100
//         char temp[100] = {0}; //4 floats of 9 and one of 8 = 45 and 35 html 
//         float econ_lt;
//         float econ_ht;
//         float eret_lt;
//         float eret_ht;
//         float mgas;
//         int next;
//         int prev;
//         int maand = month()+1;
//         int prevMonth;
//         int currentMonth = month();
//         MVALS[currentMonth].EC_LT = meter.con_lt;
//         MVALS[currentMonth].EC_HT = meter.con_ht;
//         MVALS[currentMonth].ER_LT = meter.ret_lt;
//         MVALS[currentMonth].ER_HT = meter.ret_ht;
//         MVALS[currentMonth].mGAS  = meter.gas;
//         // the monthly values are written at the very beginning of each month so month2 is end month1
//         // if the nextmonth = smaller, this is a value of a former year
//         // we can not calculate it unless x - the current month, thec we can calculate the todate value
        
//         // we have 12 values 1 to 12
//         //for mont 1 the value = 2 -/- 1 and 11 = 12 -/-11 and 12 = 1-12
//         //the struct goes fom 0 to 11
        
//         for(int x=1; x<13; x++) {
       
//         econ_lt = 0;
//         econ_ht = 0;
//         eret_lt = 0;
//         eret_ht = 0;
//         mgas = 0;       
// //        
//         next = x+1;
//         if(next == 13) next = 0; // the last iteration is thus 0 - 11   
// //        prev = x-1;
// //        if(prev == 0) prev = 12; 
//          Serial.println("x = " + String(x));
//         //if there are no values we skip this whole iteration
//         if ( MVALS[x].EC_LT != 0 || MVALS[x].EC_HT != 0 || MVALS[x].ER_LT != 0 || MVALS[x].ER_HT != 0 || MVALS[x].mGAS != 0 )
//         { 
//             Serial.println("found values in month " + String(x));
//             // say we are in the month 8 and have values
//             prevMonth = x - 1;
//             if(prevMonth == 0) prevMonth = 12;
//             econ_lt = MVALS[x].EC_LT - MVALS[prevMonth].EC_LT;
//             econ_ht = MVALS[x].EC_HT - MVALS[prevMonth].EC_HT;
//             eret_lt = MVALS[x].ER_LT - MVALS[prevMonth].ER_LT;
//             eret_ht = MVALS[x].ER_HT - MVALS[prevMonth].ER_HT;
//             mgas = MVALS[x].mGAS - MVALS[prevMonth].mGAS;
            
//             // if( MVALS[next].EC_HT != 0 &&  MVALS[next].EC_HT > MVALS[x].EC_HT ) { 
//             //    econ_ht = MVALS[next].EC_HT - MVALS[x].EC_HT;
//             // } else {
//             //    if (ECON_HT > MVALS[x].EC_HT ) econ_ht = meter.con_ht - MVALS[x].EC_HT; //
//             // }            

//             // if( MVALS[next].ER_LT != 0 &&  MVALS[next].ER_LT > MVALS[x].ER_LT ) { 
//             //    eret_lt = MVALS[next].ER_LT - MVALS[x].ER_LT;
//             // } else {
//             //    if (ERET_LT > MVALS[x].ER_LT ) eret_lt = meter.ret_lt - MVALS[x].ER_LT; //
//             // } 
            
//             // if( MVALS[next].ER_HT != 0 &&  MVALS[next].ER_HT > MVALS[x].ER_HT ) { 
//             //    eret_ht = MVALS[next].ER_HT - MVALS[x].ER_HT;
//             // } else {
//             //    if (ERET_HT > MVALS[x].ER_HT ) eret_ht = meter.ret_ht - MVALS[x].ER_HT; //
//             // }             


//             // if( MVALS[next].mGAS != 0 &&  MVALS[next].mGAS > MVALS[x].mGAS ) { 
//             //    mgas = MVALS[next].mGAS - MVALS[x].mGAS;
//             // } else {
//             //    if (mGAS > MVALS[x].mGAS ) mgas = meter.gas - MVALS[x].mGAS; //
//             // }             
          
//        } else { Serial.println("skipped line " + String(x));}    
//         // for each month we print a line
//         // we want the current month always be the last one
//         sprintf(temp, "<tr id=\"row%d\"><td>%d<td>%.2f<td>%.2f<td>%.2f<td>%.2f<td>%.2f</td></tr>", x, x, econ_lt, econ_ht, eret_lt, eret_ht, mgas);  
//         if( currentMonth >=de berekening wordt alleen gedaan als x) {
//             strcat(report2, temp); // so 9 10 11 12
//         } else { 
//             strcat(report1, temp);} //  1 - 8
//         }    
//         strcat(report1, report2); 
//         //console_Log(String(report));
//         consoleOut("length = " + String(strlen(report2)));
//         //swap_to_hw();
//         return report1;
//         }

// return String();
// }
// String putReport(const String& var)
// {
//    if(var == "rows") {
//        String report1 = "";
//        String report2 = "";
//        char temp[150] = {0}; 
       
//        int currentMonth = month();
//        int showMonth = currentMonth + 1; 
//        MVALS[currentMonth].EC_LT = meter.con_lt;
//        MVALS[currentMonth].EC_HT = meter.con_ht;
//        MVALS[currentMonth].ER_LT = meter.ret_lt;
//        MVALS[currentMonth].ER_HT = meter.ret_ht;
//        MVALS[currentMonth].mGAS  = meter.gas;

//       for(int x = 1; x < 13; x++) 
//        {
//             int next = x + 1;
//             if(next == 13) next = 1; 
            
//             float econ_lt = 0, econ_ht = 0, eret_lt = 0, eret_ht = 0, mgas = 0;       
            
//             // CHECK 1: Als x de HUIDIGE MAAND is, rekenen we ALTIJD met de live meterstand
//             // if (x == currentMonth) {
//             //     if (MVALS[x].EC_LT != 0) { // We hebben een geldige beginstand van deze maand
//             //         if(meter.con_lt > MVALS[x].EC_LT) econ_lt = meter.con_lt - MVALS[x].EC_LT;
//             //         if(meter.con_ht > MVALS[x].EC_HT) econ_ht = meter.con_ht - MVALS[x].EC_HT;
//             //         if(meter.ret_lt > MVALS[x].ER_LT) eret_lt = meter.ret_lt - MVALS[x].ER_LT;
//             //         if(meter.ret_ht > MVALS[x].ER_HT) eret_ht = meter.ret_ht - MVALS[x].ER_HT;
//             //         if(meter.gas    > MVALS[x].mGAS)  mgas    = meter.gas    - MVALS[x].mGAS;
//             //     }
//             // } 
//             // CHECK 2: Voor alle ANDERE (afgelopen) maanden gebruiken we de waarden van de volgende maand
//             //else {
//                 if (MVALS[x].EC_LT != 0 && MVALS[next].EC_LT != 0) {
//                     if(MVALS[next].EC_LT > MVALS[x].EC_LT) econ_lt = MVALS[next].EC_LT - MVALS[x].EC_LT;
//                     if(MVALS[next].EC_HT > MVALS[x].EC_HT) econ_ht = MVALS[next].EC_HT - MVALS[x].EC_HT;
//                     if(MVALS[next].ER_LT > MVALS[x].ER_LT) eret_lt = MVALS[next].ER_LT - MVALS[x].ER_LT;
//                     if(MVALS[next].ER_HT > MVALS[x].ER_HT) eret_ht = MVALS[next].ER_HT - MVALS[x].ER_HT;
//                     if(MVALS[next].mGAS  > MVALS[x].mGAS)  mgas    = MVALS[next].mGAS  - MVALS[x].mGAS;
//                 }
//             //}
          
//             // Bild the HTML row
//             sprintf(temp, "<tr id=\"row%d\"><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>", 
//                     x, x, econ_lt, econ_ht, eret_lt, eret_ht, mgas);  
            
//             if(showMonth > x) {
//                 report2 += String(temp);
//             } else { 
//                 report1 += String(temp);
//             }    
//        }    
       
//        return report1 + report2;
//     }
//     return String();
// }


// String putReport(const String& var)
// {

//    if(var == "rows") {
//    char report1[1800] = {0}; //12 x 100
//    char report2[1600] = {0}; //12 x 100
//    char temp[150] = {0}; //4 floats of 9 and one of 8 = 45 and 35 html 
//    float econ_lt;
//    float econ_ht;
//    float eret_lt;
//    float eret_ht;
//    float mgas;
//    int next;
//    int prev;
//    int maand = month()+1;
//    // the monthly values are written at the very beginning of each month so month2 is end month1
//    // if the nextmonth = smaller, this is a value of a former year
//    // we can not calculate it unless x - the current month, thec we can calculate the todate value
   
//    // we have 12 values 1 to 12
//    //for mont 1 the value = 2 -/- 1 and 11 = 12 -/-11 and 12 = 1-12
//    //the struct goes fom 0 to 11
   
//    for(int x=1; x<13; x++) 
//    {
       
//         econ_lt = 0;
//         econ_ht = 0;
//         eret_lt = 0;
//         eret_ht = 0;
//         mgas = 0;       
       
//         next = x + 1;
//         if(next == 13) next = 1; // the last iteration is thus 0 - 11   
 
//          consoleOut("x = " + String(x));
//          //if there are no values in this month we skip this whole iteration
//          if ( MVALS[x].EC_LT != 0 || MVALS[x].EC_HT != 0 || MVALS[x].ER_LT != 0 || MVALS[x].ER_HT != 0 || MVALS[x].mGAS != 0 )
//          { 
//              consoleOut("found values in month " + String(x));
//             // we are in the month 8 and have values
//             //check if there are values in the next month and they are greater
//             // if we have greater values of the next month, we could calculate
//             if( MVALS[next].EC_LT != 0 &&  MVALS[next].EC_LT > MVALS[x].EC_LT ) 
//             { 
//              consoleOut("also values in next month " + String(next));
//                    econ_lt = MVALS[next].EC_LT - MVALS[x].EC_LT;
//                    // this would be over the current month
//             } else {
//             // we have values in x but not (greater) values in next
//             // we can check if the current values are greater than the ones in the file
//             // we can calculate a todate value that would be over the current mont 
//              consoleOut("no values in next month so calculate todate"); 
//                if (meter.con_lt > MVALS[x].EC_LT ) {
//                  econ_lt = meter.con_lt - MVALS[x].EC_LT; //
//                } else { 
//                  consoleOut("could not calculate todate for " + String(x));}
//             }  
          
//             if( MVALS[next].EC_HT != 0 &&  MVALS[next].EC_HT > MVALS[x].EC_HT ) { 
//                econ_ht = MVALS[next].EC_HT - MVALS[x].EC_HT;
//             } else {
//                if (meter.con_ht > MVALS[x].EC_HT ) econ_ht = meter.con_ht - MVALS[x].EC_HT; //
//             }            

//             if( MVALS[next].ER_LT != 0 &&  MVALS[next].ER_LT > MVALS[x].ER_LT ) { 
//                eret_lt = MVALS[next].ER_LT - MVALS[x].ER_LT;
//             } else {
//                if (meter.ret_lt > MVALS[x].ER_LT ) eret_lt = meter.ret_lt - MVALS[x].ER_LT; //
//             } 
            
//             if( MVALS[next].ER_HT != 0 &&  MVALS[next].ER_HT > MVALS[x].ER_HT ) { 
//                eret_ht = MVALS[next].ER_HT - MVALS[x].ER_HT;
//             } else {
//                if (meter.ret_ht > MVALS[x].ER_HT ) eret_ht = meter.ret_ht - MVALS[x].ER_HT; //
//             }             


//             if( MVALS[next].mGAS != 0 &&  MVALS[next].mGAS > MVALS[x].mGAS ) { 
//                mgas = MVALS[next].mGAS - MVALS[x].mGAS;
//             } else {
//                if (meter.gas > MVALS[x].mGAS ) mgas = meter.gas - MVALS[x].mGAS; //
//             }             
          
//          } else { consoleOut("skipped line " + String(x));}    
//           // for each month we print a line
//           // we want the current month alwas be the last one
//          sprintf(temp, "<tr id=\"row%d\"><td>%d<td>%.2f<td>%.2f<td>%.2f<td>%.2f<td>%.2f</td></tr>", x, x, econ_lt, econ_ht, eret_lt, eret_ht, mgas);  
//          if( maand > x) {
//             strcat(report2, temp); // so 9 10 11 12
//          } else { 
//             strcat(report1, temp);} //  1 - 8
//          }    
//          strcat(report1, report2); 
//          //consoleLog(String(report));
//          consoleOut("length = " + String(strlen(report2)));
//          //swap_to_hw();
//          return report1;
//     }

// return String();
// }




void printStruct( String bestand, int what) {
//input String bestand = "/Inv_Prop" + String(x) + ".str";
      //String bestand = bestand + String(i) + ".str"
      //readStruct(bestand); is done at boottime
        consoleOut("content of MVALS[" + String(what) + "]");
        consoleOut("EC_LT = " + String(MVALS[what].EC_LT));     
        consoleOut("EC_HT = " + String(MVALS[what].EC_HT));
        consoleOut("ER_LT = " + String(MVALS[what].ER_LT));
        consoleOut("ER_HT = " + String(MVALS[what].ER_HT));
        consoleOut("mGAS = " + String(MVALS[what].mGAS));

}
