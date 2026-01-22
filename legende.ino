// nodemcu pins
//  16  D0  
//   5  D1  
//   4  D2  
//   0  D3
//   2  D4   
//   GND
//   3,3v
//   14  D5  
//   12  D6  
//   13  D7 -> P02
//   15  D8 -> P03
//   3   D9
//   1   D10

// compile settings FS: Minima SPIFFS with OTA 
/* had to modyfy AsyncWebSynchronization.h to solve a compilation error

 lots of troubles geting data om serial1, getting wrong (chinese) characters
 tried everything, 
 different port pins, 
 the latest was reducing the cpu frequency to 80 (40 didn't work) 80 no result
 baud to 9600 didn't help
 remove the inverson on the pin didn't help
 Use the native serial port Serial like the nodemcu
 */
// api 2 hase meter {"wifi_ssid":"Ziggo8831772","wifi_strength":68,"smr_version":50,"meter_model":"Sagemcom T210-D ESMR 5.0","unique_id":"4530303438303030303438363838353230","active_tariff":2,"total_power_import_kwh":16105.680,"total_power_import_t1_kwh":10118.923,"total_power_import_t2_kwh":5986.757,"total_power_export_kwh":12115.683,"total_power_export_t1_kwh":3792.337,"total_power_export_t2_kwh":8323.346,"active_power_w":-344.000,"active_power_l1_w":-524.000,"

