// deze functie berekent de zonsopkomst en zonsondergangstijden
void sun_setrise() {

    float OUR_longtitude = longi;
    float OUR_latitude = lati;
    float OUR_timezone = atof(gmtOffset); //   120                     // localtime with UTC difference in minutes
    
    sunMoon  sm;

    sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);
    time_t sunrise = sm.sunRise();
    time_t sunset  = sm.sunSet();
      
    if ( zomerTijd ) { // er is zomertijd ingesteld
          if (zomertijd() == true) { // we kijken of het zomertijd is
            sunrise = sunrise + 3600; // seconden
            sunset  = sunset + 3600;
            dst = 1; //summer time
            } else {
            dst = 2; // winter time 
            }
    } else { 
            dst = 0; 
    }
 }


// *********** this function determines if dts applies
bool zomertijd() {
    int m = month();
    int d = day();

    // Shortcut: November to February is guaranteed wintertime
    // April t/m September is guaranteed summertime
    if (m < 3 || m > 10) return false;
    if (m > 3 && m < 10) return true;

    // We only need to caclculate March (3) and Oct (10)!
    int j = year();
    
    if (m == 3) {
        // Direct formula for the last sunday in march
        int lastSundayMarch = 31 - dow(j, 3, 31);
        return d >= lastSundayMarch; // true or false
    }
    
    if (m == 10) {
        // Direct formula for the last sunday in oct
        int lastSundayOct = 31 - dow(j, 10, 31);
        return d < lastSundayOct; // true or false
    }

    return false;
}

int dow(int y, int m, int d) {
static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
y -= m < 3;
return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}


