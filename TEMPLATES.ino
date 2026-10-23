// we kunnen test4en of we at boot een eenmalige volledig poll kunnen doen
// en onderdelen daarvan in een sjabloon kunnen plaatsen.
//bool templateMade;
//String jsonPart1 = ""; // contains the non volatile data at the start untill the live data
//String jsonPart2 = ""; // contains the non volatile data at the tail

// void makeJsonTemplate() {
//   // Bereken eenmalig de waarden die bij boot vaststaan
//   int rssi = WiFi.RSSI();
//   int wifi_strength = 2 * (rssi + 100);
//   if(wifi_strength > 100) wifi_strength = 100;
//   if(wifi_strength < 0) wifi_strength = 0;

//   //     root["smr_version"] = 50;
// //     root["meter_model"] = String(meterType); // moet
// //     root["wifi_ssid"] = WiFi.SSID();
// //     root["wifi_strength"] = WiFi.RSSI();
// //     root["total_power_import_t1_kwh"] = round3(meter.con_ht); // tariff 1
// //     root["total_power_import_t2_kwh"] = round3(meter.con_lt); // tariff 2
// //     root["total_power_export_t1_kwh"] = round3(meter.ret_ht); // tariff 1
// //     root["total_power_export_t2_kwh"] = round3(meter.ret_lt); // tariff 2
  
  
//   // === DEEL 1: Exact de volgorde van de echte dongle tot aan de live wattages ===
//   jsonPart1 = "{\n";
//   jsonPart1 += "  \"wifi_ssid\": \"" + WiFi.SSID() + "\",\n";
//   jsonPart1 += "  \"wifi_strength\": " + String(wifi_strength) + ",\n";
//   jsonPart1 += "  \"smr_version\": 50,\n";
//   jsonPart1 += "  \"meter_model\": \"" + String(meterType) + "\",\n";
//   jsonPart1 += "  \"unique_id\": \"" + String(meter.uid) + "\",\n";
//   jsonPart1 += "  \"active_tariff\": " + String(meter.tariff) + ",\n"; // 1 of 2
//   jsonPart1 += "  \"total_power_import_kwh\": " + String((meter.con_ht + meter.con_lt), 3) + ",\n";
//   jsonPart1 += "  \"total_power_import_t1_kwh\": " + String(meter.con_lt, 3) + ",\n";
//   jsonPart1 += "  \"total_power_import_t2_kwh\": " + String(meter.con_ht, 3) + ",\n";
//   jsonPart1 += "  \"total_power_export_kwh\": "    + String((meter.ret_ht + meter.ret_lt), 3) + ",\n";
//   jsonPart1 += "  \"total_power_export_t1_kwh\": " + String(meter.ret_lt, 3) + ",\n";
//   jsonPart1 += "  \"total_power_export_t2_kwh\": " + String(meter.ret_ht, 3) + ",\n";

//   templateMade = true;
// }
void makeJsonTemplateHW() 
{

    int rssi = WiFi.RSSI();
    int wifi_strength = 2 * (rssi + 100);
    if(wifi_strength > 100) wifi_strength = 100;
    if(wifi_strength < 0) wifi_strength = 0;
   
    float total_import = meter.con_ht + meter.con_lt;
    float total_export = meter.ret_ht + meter.ret_lt;

    snprintf(jsonApi, sizeof(jsonApi),
        "{\n"
        "  \"wifi_ssid\": \"%s\",\n"
        "  \"wifi_strength\": %d,\n"
        "  \"smr_version\": 50,\n"
        "  \"meter_model\": \"%s\",\n"
        "  \"unique_id\": \"%s\",\n"
        "  \"active_tariff\": %d,\n"
        "  \"total_power_import_kwh\": %.3f,\n"
        "  \"total_power_import_t1_kwh\": %.3f,\n"
        "  \"total_power_import_t2_kwh\": %.3f,\n"
        "  \"total_power_export_kwh\": %.3f,\n"
        "  \"total_power_export_t1_kwh\": %.3f,\n"
        "  \"total_power_export_t2_kwh\": %.3f,\n", // Let op de komma aan het einde!
        WiFi.SSID().c_str(), wifi_strength, meterType, meter.uid, meter.tariff,
        total_import, meter.con_lt, meter.con_ht,
        total_export, meter.ret_lt, meter.ret_ht
    );
    templateMade = true;
}

// Bouwt de WebUI JSON cache (voor de frontpage)
void makeJsonTemplateWU() {
    // 1. Bereken de totalen voor de WebUI frontpage
    float enReturn = meter.ret_ht + meter.ret_lt;
    float enCons   = meter.con_ht + meter.con_lt;

    // 2. Vul de webUiCacheBuffer overzichtelijk in één klap
    snprintf(jsonWebui, sizeof(jsonWebui),
        "{\n"
        "  \"timestamp\": \"%s\",\n"
        "  \"CON_HT\": %.3f,\n"
        "  \"CON_LT\": %.3f,\n"
        "  \"RET_HT\": %.3f,\n"
        "  \"RET_LT\": %.3f,\n"
        "  \"PWRC1\": %d,\n"
        "  \"PWRR1\": %d,\n"
        "  \"PWRC2\": %d,\n"
        "  \"PWRR2\": %d,\n"
        "  \"PWRC3\": %d,\n"
        "  \"PWRR3\": %d,\n"
        "  \"enR\": %.3f,\n"
        "  \"enC\": %.3f,\n"
        "  \"gAs\": %.3f,\n"
        "  \"threeP\": %d", // Let op: GEEN sluitaccolade en GEEN regeleinde hier!
        timeStamp, meter.con_ht, meter.con_lt, meter.ret_ht, meter.ret_lt,
        meter.pwr_con[0], meter.pwr_ret[0], 
        meter.pwr_con[1], meter.pwr_ret[1], 
        meter.pwr_con[2], meter.pwr_ret[2],
        enReturn, enCons, meter.gas, threePhase
    );
    templateMade = true;
}