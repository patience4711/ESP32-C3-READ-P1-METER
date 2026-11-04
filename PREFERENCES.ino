void writeMonths() {
  
  // first load the current metervalues in the struct
  int maand = month() - 1; // af this is 1, maand would be 0
  if (maand == 0) {maand = 12;}
  MVALS[maand].EC_LT = meter.con_lt ;
  MVALS[maand].EC_HT = meter.con_ht ;
  MVALS[maand].ER_LT = meter.ret_lt ;
  MVALS[maand].ER_HT = meter.ret_ht ;
  MVALS[maand].mGAS  = meter.gas;

  prefs.begin("p1meter", false); 
  
  // save the whole array MVALS (13x struct) as one block bytes
  prefs.putBytes("mvals_array", &MVALS, sizeof(MVALS));
  
  prefs.end();
  consoleOut("Monthly readings saved in Preferences!");
    // 3. Bouw de logstring op volgens Optie 3 (snprintf)
  
}
void writeMonthsTest() {
  
  prefs.begin("p1meter", false); 
  
  // save the whole array MVALS (13x struct) as one block bytes
  prefs.putBytes("mvals_array", &MVALS, sizeof(MVALS));
  
  prefs.end();
  consoleOut("writeMonthsTest readings saved in Preferences!");
    // 3. Bouw de logstring op volgens Optie 3 (snprintf)
  
}
// void writeMonths(bool endMonth) {
  
//   // first load the current metervalues in the struct
//   int maand = month(); // af this is 1, maand would be 0
//   if(endMonth) {
//     maand --;
//     if (maand == 0) maand = 12; // if januari, write to december (12)
//     memset(logMonth, 0, sizeof(logMonth));

//     // write directly into the global logMonth
//     snprintf(logMonth, sizeof(logMonth), 
//            "[%02d:%02d:%02d %02d-%02d-%04d] wrote monthly mvals for month: %d", 
//            hour(), minute(), second(), day(), month(), year(), maand);
//   } else {
//     memset(logDay, 0, sizeof(logDay));
//     snprintf(logDay, sizeof(logDay), 
//            "[%02d:%02d:%02d %02d-%02d-%04d] wrote dayly mvals for month: %d", 
//            hour(), minute(), second(), day(), month(), year(), maand);
//   }
  
   
//   MVALS[maand].EC_LT = meter.con_lt ;
//   MVALS[maand].EC_HT = meter.con_ht ;
//   MVALS[maand].ER_LT = meter.ret_lt ;
//   MVALS[maand].ER_HT = meter.ret_ht ;
//   MVALS[maand].mGAS  = meter.gas;
  
//   prefs.begin("p1meter", false); 
  
//   // save the whole array MVALS (13x struct) as one block bytes
//   prefs.putBytes("mvals_array", &MVALS, sizeof(MVALS));
  
//   prefs.end();
//   consoleOut("Monthly readings saved in Preferences!");
//     // 3. Bouw de logstring op volgens Optie 3 (snprintf)
  
// }

void readMonths() {
  memset(&MVALS, 0, sizeof(MVALS));
  
  prefs.begin("p1meter", true); // Open in read-only mode (true)
  
  // check if data exists and the length is right
  if (prefs.getBytesLength("mvals_array") == sizeof(MVALS)) {
    prefs.getBytes("mvals_array", &MVALS, sizeof(MVALS));
    consoleOut("monthly readings successfully loaded.");
  } else {
    consoleOut("no monthly readings, table stays 0.");
  }
  
  prefs.end();
}