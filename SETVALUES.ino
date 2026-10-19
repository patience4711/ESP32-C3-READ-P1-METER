const char VALUECONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'><div id='bo'></div>
<div id="menu">
<a href="#" id="sub" onclick='submitFunction()'>save</a>
<a href="#" class='close' onclick='cl();'>&times;</a>
</div>
</div>
<div id='msect'>
<br><kop>P1METER SET VALUES</kop><br><br>
</div>
<div id='msect'> 
  <div class='divstijl' style='height:52vh;'>
  <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  <center><table>
  <tr><td style='width:140px;'>energy cons ht<td><input class='inp4' name='econht' value='{echt}'></input></tr>

<tr><td>energy cons lt<td><input  class='inp4' name='econlt'  value='{eclt}'></input></tr> 

<tr><td>energy ret ht<td><input class='inp4' name='eretht' value='{erht}'></input></tr>

 <tr><td>energy ret lt<td><input class='inp4' name='eretlt' value='{erlt}'></input></tr>
 <tr><td>gas usage <td><input class='inp4' name='mgas' value='{mgas}'></input></tr>
<tr><td>for month<td><input type='number' min='1' max='12' class='inp1' name='mnr' value='{mnr}'></input></tr>

</table> 
  
  </div></div><br></form>
<br></body></html>

)=====";



void zendPageVALUEconfig( AsyncWebServerRequest *request ) {

    String webPage = FPSTR(HTML_HEAD);
    webPage += FPSTR(VALUECONFIG);  
    webPage.replace("{mnr}",  String(month()) ); 
    webPage.replace("{echt}",  String(meter.con_ht,3) );
    webPage.replace("{eclt}",  String(meter.con_lt,3) );
    webPage.replace("{erht}",  String(meter.ret_ht,3) );
    webPage.replace("{erlt}",  String(meter.ret_lt,3) );
    webPage.replace("{mgas}",  String(meter.gas,3) );    
    request->send(200, "text/html", webPage);
    webPage="";
}
