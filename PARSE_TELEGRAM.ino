
void parseTelegram(bool fast)
{
    //unsigned long startParse = millis();
    const char* p = teleGram;
    consoleOut("parsing teleGram");
    while (*p)
    {
        // ==========================================
        // SNELLE MODUS: Sla alle vaste totalen over!
        // ==========================================
        if (fast) 
        {
            // We check DIRECT only the live data per phase (C1, R1, C2, R2, C3, R3)
            if (!strncmp(p, OBIS_POWER_C1, strlen(OBIS_POWER_C1)))      meter.pwr_con[0] = (uint16_t)(parseValue(p, OBIS_POWER_C1) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R1, strlen(OBIS_POWER_R1))) meter.pwr_ret[0] = (uint16_t)(parseValue(p, OBIS_POWER_R1) * 1000.0f);
            else if (!strncmp(p, OBIS_POWER_C2, strlen(OBIS_POWER_C2))) meter.pwr_con[1] = (uint16_t)(parseValue(p, OBIS_POWER_C2) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R2, strlen(OBIS_POWER_R2))) meter.pwr_ret[1] = (uint16_t)(parseValue(p, OBIS_POWER_R2) * 1000.0f);
            else if (!strncmp(p, OBIS_POWER_C3, strlen(OBIS_POWER_C3))) meter.pwr_con[2] = (uint16_t)(parseValue(p, OBIS_POWER_C3) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R3, strlen(OBIS_POWER_R3))) meter.pwr_ret[2] = (uint16_t)(parseValue(p, OBIS_POWER_R3) * 1000.0f); 
        } 
        // ==========================================
        // SLOw MODUS (at boot): read all
        // ==========================================
        else 
        {
            // DSMR version
            if (!strncmp(p, OBIS_SMR, strlen(OBIS_SMR)))  meter.smr = (uint8_t)parseValue(p, OBIS_SMR);
            // Energy consumed
            //else if (!strncmp(p, OBIS_UID, strlen(OBIS_UID))) meter.uid = parseValue(p, OBIS_UID);
            else if (!strncmp(p, OBIS_UID, strlen(OBIS_UID))) {
                const char* start = strchr(p, '(');
                if (start) {
                    start++; 
                    int i = 0;
                    while (*start && *start != ')' && i < 34) {
                        meter.uid[i] = *start;
                        start++;
                        i++;
                    }
                    meter.uid[i] = '\0'; // Sluit de C-string netjes af!
                }
            }
            else if (!strncmp(p, OBIS_TAR, strlen(OBIS_TAR))) meter.tariff = (int)parseValue(p, OBIS_TAR);
            else if (!strncmp(p, OBIS_CON_LT, strlen(OBIS_CON_LT))) meter.con_lt = parseValue(p, OBIS_CON_LT);
            else if (!strncmp(p, OBIS_CON_HT, strlen(OBIS_CON_HT))) meter.con_ht = parseValue(p, OBIS_CON_HT);
            // Energy returned
            else if (!strncmp(p, OBIS_RET_LT, strlen(OBIS_RET_LT))) meter.ret_lt = parseValue(p, OBIS_RET_LT);
            else if (!strncmp(p, OBIS_RET_HT, strlen(OBIS_RET_HT))) meter.ret_ht = parseValue(p, OBIS_RET_HT);
            // power per phase
            else if (!strncmp(p, OBIS_POWER_C1, strlen(OBIS_POWER_C1))) meter.pwr_con[0] = (uint16_t)(parseValue(p, OBIS_POWER_C1) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R1, strlen(OBIS_POWER_R1))) meter.pwr_ret[0] = (uint16_t)(parseValue(p, OBIS_POWER_R1) * 1000.0f);
            else if (!strncmp(p, OBIS_POWER_C2, strlen(OBIS_POWER_C2))) meter.pwr_con[1] = (uint16_t)(parseValue(p, OBIS_POWER_C2) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R2, strlen(OBIS_POWER_R2))) meter.pwr_ret[1] = (uint16_t)(parseValue(p, OBIS_POWER_R2) * 1000.0f);
            else if (!strncmp(p, OBIS_POWER_C3, strlen(OBIS_POWER_C3))) meter.pwr_con[2] = (uint16_t)(parseValue(p, OBIS_POWER_C3) * 1000.0f); 
            else if (!strncmp(p, OBIS_POWER_R3, strlen(OBIS_POWER_R3))) meter.pwr_ret[2] = (uint16_t)(parseValue(p, OBIS_POWER_R3) * 1000.0f); 
            // Gas
            else if (!strncmp(p, OBIS_GAS, strlen(OBIS_GAS))) meter.gas = parseGasValue(p);                              
        }

        // move to next line
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++; // now we are on the next line
    }
    //parseDurationMs = millis() - startParse;
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