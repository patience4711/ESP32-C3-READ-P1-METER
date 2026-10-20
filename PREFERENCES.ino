void writeMonths() {
  
  // first load the current metervalues in the struct
  int maand = month();
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
}

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