
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