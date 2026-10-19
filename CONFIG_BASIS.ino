//{ check

//    webPage.replace("{domAdres}",    String(dom_Address)   );
//    webPage.replace("{domPort}",     String(dom_Port)     );
//<tr><td >domoticz ip<td><input class='inp6' name='domAdres' value='{domAdres}' size='31' placeholder='broker adres'></tr>
//  <tr><td >domoticz port<td><input class='inp2' name='domPort' value='{domPort}' size='31' placeholder='dom port'></tr>
//  <tr><td class="cap">meter type<td><select name='mtype' class='sb1' id='sel' >
//    <option value='0' mtype_0>NO METER</option>
//    <option value='1' mtype_1>XS210_D ESMR5</option>
//    <option value='2' mtype_2>LANDIS GYR E350 ZMF100</option></select>
//    </tr>

  // <tr><td class="cap">auto polling freq<td><select name='pfreq' class='sb1' id='sel2' >
  //   <option value ='0' pfreq_0>no polling
  //   <option value='30' pfreq_1>every 30 sec</option>
  //   <option value='60' pfreq_2>every 1 min</option>
  //   <option value='300' pfreq_3>every 5 min</select>
  //   <option value='3' pfreq_4>on demand 3</select>
  //   <option value='4' pfreq_5>on demand 4</select>
  //   <option value='5' pfreq_6>on demand 5</select>
  //   <option value='6' pfreq_7>on demand 6</select>
  //   <option value='7' pfreq_8>on demand 7</select>
  //   </tr>


const char BASISCONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'>
<div id="menu">

<a href="#" id="sub" onclick='submitFunction()'>save</a>
<a href="#" class='close' onclick='cl();'>&times;</a>
</div>
<kop>P1-METER SETTINGS</kop>
  <div class='divstijl' style='width: 480px; height:56vh;'>
  <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  <center><table>
    <tr><td>user passwd<td><input  class='inp5' name='pw1' length='11' readonly value='1111'  title='between 4 en 10 characters'></input> 
  </td></tr>
  <tr><td class="cap">meter type<td><select name='mType' class='sb1' id='sel' >
    <option value='no meter' mtype_0>NO METER</option>
    <option value='Sagemcom' mtype_1>SAGEMCOM XS210_D ESMR5</option>
    <option value='Landis Gyr' mtype_2>LANDIS GYR E350 ZMF100</option></select></tr> 
  <tr><td>3phase meter:<td><input type='checkbox' style='width:30px; height:30px;' name='3ph' #sjak ></input></td></tr>
  <tr><td>baud 9600:<td><input type='checkbox' style='width:30px; height:30px;' name='brt' #sjuk ></input></td></tr>
  <tr><td>rx invert:<td><input type='checkbox' style='width:30px; height:30px;' name='rxi' #sjik ></input></td></tr>
  
  <tr><td class="cap">auto polling freq<td><select name='pfreq' class='sb1' id='sel2'>
  <option value='3'>on demand 3</option>
  <option value='4'>on demand 4</option>
  <option value='5'>on demand 5</option>
  <option value='6'>on demand 6</option>
  <option value='7'>on demand 7</option>
  <option value='30'>30 sec</option>
  <option value='60'>1 min</option>
  <option value='300'>5 min</option>
</select>{{SELECTED_PFREQ}}
   
    <tr><td>serial debug:<td><input type='checkbox' style='width:30px; height:30px;' name='debug' #sjek ></input></td></tr>
    <tr><td>led signals:<td><input type='checkbox' style='width:30px; height:30px;' name='lsig' #check></input></td><tr>
    </table></form>
  </table>
  </div><br>
</div>
</body></html>
)=====";

void zendPageBasis(AsyncWebServerRequest *request) {
  String(webPage)="";
    //if(USB_serial) Serial.println("zendPageBasis");
    webPage = FPSTR(HTML_HEAD);
    webPage += FPSTR(BASISCONFIG);

    if(String(meterType) == "no meter") {webPage.replace("mtype_0", "selected");}
     else if(String(meterType) == "Sagemcom") {webPage.replace("mtype_1", "selected");}
     else if(String(meterType) == "Landis Gyr") {webPage.replace("mtype_2", "selected");} 
 
    String script = "<script>document.getElementById('sel2').value = '" + String(pollFreq) + "';</script>";
    webPage.replace("{{SELECTED_PFREQ}}", script);
     
    if (ledSignal) { webPage.replace("#check", "checked"); }
    if (diagNose){ webPage.replace("#sjek","checked"); }
    if (threePhase){ webPage.replace("#sjak","checked"); }
    if (baud9600){ webPage.replace("#sjuk","checked"); }
    if (rxInvert){ webPage.replace("#sjik","checked"); }
    request->send(200, "text/html", webPage);
    webPage=""; // free up
}


