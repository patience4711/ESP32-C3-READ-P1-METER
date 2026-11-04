/* struct MeterData {
    uint8_t smr;
    char uid[35];
    int tariff;
    float   con_lt;
    float   con_ht;
    float   ret_lt;
    float   ret_ht;
    uint16_t   pwr_con[3];
    uint16_t   pwr_ret[3];
    float   gas;
};
*/
#ifdef TESTCONFIG

void fakePoll(bool fast)
{
  if(ledSignal) my_wifiConnect.ledOn(50,0,0);
      meter.con_ht = 2714.333;
      meter.con_lt = 2006.444;
      meter.ret_ht = 525.555;
      meter.ret_lt = 933.666;
      meter.pwr_con[0] = 207.289; 
      meter.pwr_con[1] = 0.0;
      meter.pwr_con[2] = 0.0;
      meter.pwr_ret[0] = 333.444;
      meter.pwr_ret[1] = 0;
      meter.pwr_ret[2] = 0;
  
  makeJsonTemplateHW();
  makeJsonTemplateWU();
  
  if (fast) delay(90); else delay(150);
  
  if(ledSignal) my_wifiConnect.ledOff();

}

#endif
// void testPrint(int aantal) {
//   String inputString = "";
//   char inComing[2];
//   consoleOut("print first 20 characters");
//   for(int x=0; x<aantal; x++) {
//   Serial1.read(inComing,1);
//   //inputString += data;
//   consoleOut(String(inComing[1]));
//   //consoleLog(inputString);
//   }
// }


    
