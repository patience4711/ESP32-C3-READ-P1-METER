const char VALUECONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'><div id='bo'></div>
<div id="menu">
<a href="#" id="sub" onclick='submitFunction()'>save</a>
<a href="#" class='close' onclick='cl();'>&times;</a>
</div>
</div>
<div id='help'>
<span class='close' onclick='sluit()'>&times;</span>
Here you can edit monthly readings.<br>
the values in energy should be values sim 
</div>
<div id='msect'>
<br><kop>P1METER SET VALUES</kop><br><br>
</div>
<div id='msect'> 
  <div class='divstijl' style='height:52vh;'>
  <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  <center><table>
  <tr><td>for month<td><input type='number' min='1' max='12' class='inp1' id='mnr' name='mnr' value='1' onchange='loadMonthData()'></input></tr>
  
  <tr><td>energy cons lt<td><input  class='inp4' name='econlt' id='econlt'  value='{eclt}'></input></tr>
  <tr><td style='width:140px;'>energy cons ht<td><input class='inp4' name='econht' id='econht' value='{echt}'></input></tr>
  
  <tr><td>energy ret lt<td><input class='inp4' name='eretlt' id='eretlt' value='{erlt}'></input></tr>
  <tr><td>energy ret ht<td><input class='inp4' name='eretht' id='eretht' value='{erht}'></input></tr>

  <tr><td>gas usage <td><input class='inp4' name='mGas' id='mGas' value='{mgas}'></input></tr>

</table> 
  
  </div></div><br></form>
<script>
function loadMonthData() {
  var mnr = document.getElementById('mnr').value;
  
  // get the data on-the-fly from the server
  fetch('/GETMONTH?mnr=' + mnr)
    .then(response => response.json())
    .then(data => {
      // fill the inputs direct with the values from MVALS
      document.getElementById('econht').value = data.econht.toFixed(3);
      document.getElementById('econlt').value = data.econlt.toFixed(3);
      document.getElementById('eretht').value = data.eretht.toFixed(3);
      document.getElementById('eretlt').value = data.eretlt.toFixed(3);
      document.getElementById('mGas').value   = data.mgas.toFixed(3);
    });
}

// call the function at pageload for month 1
window.onload = loadMonthData;
</script>

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
