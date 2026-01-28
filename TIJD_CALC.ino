// deze functie berekent de zonsopkomst en zonsondergangstijden
void sun_setrise() {

    float OUR_longtitude = longi;
    float OUR_latitude = lati;
    float OUR_timezone = atof(gmtOffset); //   120                     // localtime with UTC difference in minutes
    
    sunMoon  sm;

//  tmElements_t  tm;                             // specific time
//  tm.Second = 0;
//  tm.Minute = 12;
//  tm.Hour   = 12;
//  tm.Day    = 3;
//  tm.Month  = 8;
//  tm.Year   = 2016 - 1970;
//  time_t s_date = makeTime(tm);
//  Serial.println("RTC has set the system time");
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);


//  uint32_t jDay = sm.julianDay();               // Optional call
//  mDay = sm.moonDay();
// if (mDay < 13){ maan = "cresc. moon";} //crescent moon"
// if (mDay > 15){maan = "waning moon";}
// if (mDay == 13 || mDay == 14 || mDay == 15){maan = "full moon";}
// if (mDay == 0 || mDay == 1 || mDay == 28){maan = "new moon";} 

      time_t sunrise = sm.sunRise();
      time_t sunset  = sm.sunSet();
      Serial.println("tijd-calc DTS = " + String(DTS));
      if ( DTS && isSummertime()) 
      { 
          sunrise = sunrise + 3600; // seconden
          sunset  = sunset + 3600;  
      } 
 }

// new way to detect sumer or wintertime
bool isSummertime() {
  time_t t = now();
  int y = year(t);
  // we calculate the time at Last Sunday of March
  tmElements_t tm;
  tm.Year = y - 1970;
  tm.Month = 3;
  tm.Day = 31;
  tm.Hour = 1;
  tm.Minute = 0;
  tm.Second = 0;
  time_t start = makeTime(tm) - weekday(makeTime(tm)) * SECS_PER_DAY;

  // we calculate the time at Last Sunday of October
  tm.Month = 10;
  tm.Day = 31;
  time_t end = makeTime(tm) - weekday(makeTime(tm)) * SECS_PER_DAY;

  return (t >= start && t < end); // true if t =s between start and end 
}


// // *********** this function determines if dts applies
// bool zomertijd() {

//     int eerstemrt = dow(year(), 3 ,1);
//     int zdmrt;

//     if (eerstemrt == 0) {
//      zdmrt = 1;
//     } else {
//      zdmrt = 1 + (7 - eerstemrt);
//     }

//     while(zdmrt <= 24){
//       zdmrt = zdmrt + 7;
//     }

//     int eersteoct = dow(year(), 10 ,1);
//     int zdoct ;

//     // dow goes from 0 to 6, sunday is 0
//     if (zdoct == 0) {
//     zdoct = 1;
//     } else {
//     zdoct = 1+(7-eersteoct);
//     }
//     while(zdoct <= 24){
//       zdoct = zdoct + 7;
//     }

//     if(((month() == 3 and day() >= zdmrt) or month() > 3) and ((month() == 10 and day() < zdoct) or month() < 10)) {  

//     return true;
//     } else {

//     return false; 
//     }
// }

// int dow(int y, int m, int d) {
// static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
// y -= m < 3;
// return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
// }
