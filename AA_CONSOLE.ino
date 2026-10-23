//<link rel="icon" type="image/x-icon" href="/favicon.ico" />

const char CONSOLE_HTML[] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<title>ESP32-P1 METER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<script>
function helpfunctie() {
document.getElementById("hulp").style.display = "block";
}
function sl() {  
document.getElementById("hulp").style.display = "none";
}

</script>

<style>
 tr {height:16px !important;
 font-size:15px !important;
 } 
 li a:hover {
   background-color: #333 !important;
}
#hulp {
  background-color: #ffffff; 
  border: solid 2px; 
  display:none; 
  padding:4px;
  width:94vw;
}
.divstl { width: 60vw; height:84vh; background: #dbd89c; border:1px solid; padding-left:10px;
}
@media only screen and (max-width: 800px) {
  .divstl { width:98%; }
  }


</style>
</head>
<body>
  <div id='hulp'>
  <span class='close' onclick='sl();'>&times;</span><h3>CONSOLE COMMANDS</h3>
  <b>PRINT-SPIFFS: </b> show filesystem<br><br>
  <b>DELETE-FILE=/filename: </b> delete a file.<br><br>
  <b>POLL-METER: </b> poll the p1 meter now <br><br>
  <b>TEST-MOSQUITTO: </b>sends a mqtt testmessage<br><br>
  <b>DUMP-VARS: </b>show some system variables<br><br>
  <b>CLEAR-CONSOLE: </b> clear console window<br><br>

  </div>

<div id='msect'>
<div id='menu'>
<a href='/MENU' onclick='confirmExit()' class='close'>&times;</span></a>
<a href='#' onclick='helpfunctie()'>help</a>
<a><input type="text" placeholder="type here" id="tiep"></a>
</div>  
<br>  
  <div class='divstl'>
  <table id='tekstveld'></table>
  </div>
 </div>

<script>
  var field = document.getElementById('tekstveld');
  var gateway = `${(window.location.protocol == "https:"?"wss":"ws")}://${window.location.hostname}/ws`;
  var websocket;
  var inputField = document.getElementById('tiep');

 
 let lastInput = ""; 

  // listen to the submit (e.g. enter)
  inputField.addEventListener("keypress", function(event) {
      if (event.key === "Enter") {
          if (inputField.value.trim() !== "") {
              lastInput = inputField.value; // save the input in memory
          }
      }
  });

  // listen to the arrow_up button
  inputField.addEventListener("keydown", function(event) {
      if (event.key === "ArrowUp") {
          if (lastInput !== "") {
              event.preventDefault(); // prevent that the cursor jumps to the begin of the row
              inputField.value = lastInput; // put the previous text back
          }
      }
  });
 
 
 
 
 
  window.onbeforeunload = confirmExit;
  function confirmExit()
  {
      alert("close the console?");
      ws.close();  
  }  
  
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection opened * *");
    inputField.focus();
    }
  function onClose(event) {
    console.log('Connection closed');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection closed * *");
    //setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    //var message = event.data;
    field.insertAdjacentHTML('beforeend', "<tr><td>" + event.data );
    if (field.rows.length > 20) {
    var rtm = field.rows.length - 20;
    for (let x=0; x<rtm; x++) { field.deleteRow(0); }
  }
    if (event.data == "clearWindow") { 
    for (let i = 0; i < 22; i++) {
        field.deleteRow(0); }
    }
   }
 
  function onLoad(event) {
    initWebSocket();
    sendEvent();
  }

  function sendEvent() {
    inputField.addEventListener('keyup', function(happen) {
    if (happen.keyCode === 13) {
       happen.preventDefault();
       sendData();
       }   
    });
  }  
  function sendData(){
  var data = inputField.value; 
  websocket.send(data, 1);
  inputField.value = "";
  }

function disConnect() {
  alert("close the console");
  ws.close(); 
}
</script>
</body>
</html>
)=====";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  if (diagNose != 0) diagNose = 1;
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  for(int i=0; i<len; i++ ) 
  {
  txBuffer[i] = data[i];
  }
  txBuffer[len]='\0'; // terminate the array

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
  {
      data[len] = 0;
            


 // ************  test mosquitto *******************************          
           if (strcasecmp(txBuffer,"TEST-MOSQUITTO") == 0) {  
              ws.textAll("test mosquitto");
              actionFlag=49; // perform the healthcheck
              diagNose=true;
              return;             
          } else 

          // see files
           if (strcasecmp(txBuffer,"PRINT-SPIFFS") == 0) {  
              //we do this in the loop
              actionFlag = 46;
              return;             
          
          } else 

           if (strcasecmp(txBuffer,"CLEAR-CONSOLE") == 0) {  
              ws.textAll("clearWindow");
              return;             
          } else           
          
          if (strncasecmp(txBuffer,"DELETE-FILE=",12) == 0)
          {  
              //input can be DELETE-FILE=filename
              ws.textAll("len = " + String(len));
              String rawInput = String(txBuffer);
              String bestand = rawInput.substring(12);
              bestand.trim();
              //String bestand="";
              //for(int i=12;  i<len+1; i++) { bestand += String(txBuffer[i]); }
               ws.textAll("bestand = " + bestand); 
               //const char* testBestand = "/mvalues_8.str";
               //SPIFFS.remove(testBestand)
               
                           
              if (SPIFFS.exists(bestand)) 
              {
                  ws.textAll("going to delete file " + bestand); 
                  {
                      SPIFFS.remove(bestand);
                      ws.textAll("file " + bestand + " removed!"); 
                  }
              
              } else 
              { 
                 ws.textAll("no such file, forgot the / ??");
              }
              return;                        
 
          } else

          if (strcasecmp(txBuffer, "DUMP-VARS") == 0) 
            {   
                ws.textAll("templateMade =" + String(templateMade)) ;  
                //ws.textAll("fastPoll = " + String(fastPoll)) ;
                ws.textAll("pollFreq = " + String(pollFreq)) ;
                ws.textAll("pollCounter = " + String(pollCounter)) ;
                ws.textAll("poll duration: " + String(pollDurationMs) + " ms");
                ws.textAll("read duration: " + String(readDurationMs) + " ms");
                //ws.textAll("decode duration: " + String(decodeDurationMs) + " ms");
                //ws.textAll("parse duration: " + String(parseDurationMs) + " ms");
                //ws.textAll("api duration: " + String(apiDurationMs) + " ms");
          } else 

          if (strcasecmp(txBuffer, "POLL-METER") == 0) 
          {
              actionFlag = 26;
              ws.textAll("going to poll the meter") ;  
        
          } else {      
      
      
       ws.textAll("unknown command"); 
      }
  
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    //Serial.println("onEvent triggered");
    switch (type) {
      case WS_EVT_CONNECT:
        //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        //Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        //Serial.println("WebSocket received data");
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
