
// from row 113 <script type="text/javascript" src="SECURITY"></script>
const char P1_HOMEPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head>

<meta charset='utf-8' name="viewport" content="width=device-width, initial-scale=1">
<title>ESP-P1 METER</title>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET">  


<style>
body {
 background-color: #EEE;
}
span {
  padding: 6px;
}
table, th, td {
  border: 2px solid blue; font-size:16px; padding:6px; text-align:center; border-collapse:collapse;backgound-color:#dfff80;
  }
tr {background-color:#ccffcc;}
//td { width:70px; }

#td0 {width:130px;}
#td1 {width:110px;}
#td2 {width:40px;}
#td3 {width:70px;}


.btn {
  background-color: #199319;
  color: white;
  padding: 5px 22px;
  border-radius:6px;
}

.btn:hover {background: #eeeF; color:black;}

/* Popup container - can be anything you want */
.popup {
  position: relative;
  display: inline-block;
  cursor: pointer;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

/* The actual popup */
.popup .popuptext {
  visibility: hidden;
  width: max-content;
  white-space:nowrap;
  background-color: #090963;
  color: #fff;
  text-align: left;
  border-radius: 6px;
  padding: 8px 12px;
  padding-left:20px;
  position: absolute;
  z-index: 1;
  bottom: 125%;
  right: 0%;
}

/* Popup arrow */
.popup .popuptext::after {
  content: "";
  position: absolute;
  top: 100%;
  right: 25px;
  margin-left: -5px;
  border-width: 5px;
  border-style: solid;
  border-color: #555 transparent transparent transparent;
}

/* Toggle this class - hide and show the popup */
.popup .show {
  visibility: visible;
  -webkit-animation: fadeIn 1s;
  animation: fadeIn 1s;
}

/* Add animation (fade in the popup) */
@-webkit-keyframes fadeIn {
  from {opacity: 0;} 
  to {opacity: 1;}
}

@keyframes fadeIn {
  from {opacity: 0;}
  to {opacity:1 ;}
}

.phase-row {
  display: none;
}

@media only screen and (max-width: 800px) {
th, td { font-size:11px; }
#td0 {width:90px}
#td1 {width:100px;}
#td2 {width:40px;}
#td3 {width:80px}

  .popup .popuptext {
       padding: 8px 12px;
  }

tr {height:35px;} 
.btn { padding: 5px 18px; font-size:10px;}
}
</style>


</head>

<body onload='loadScript()'>

<div id='msect'>
<div id='menu'>
<a id="ml" href='/MENU' style='float:right; display:none'>menu</a>
</div>
</div>
<div id='msect'>
  <kop>ESP32C3 P1 METER</kop>
</div>
<div id='msect'>
    <div class='divstijl' id='maindiv'>
        <center>
        <br>
        <h4>ELECTRICITY / GAS @ <span id="time"></span></h4>
        <br>
        <div id='noOutput' style='display:block'>
          <h4 style='color:red'>waiting for output</h4>
        </div>
        
        <div id="4channel">
          <center><table>
          <tr style='Background-color:lightblue; font-weight:bold; text-align:center; border:4px solid black;'>
          <td id="td0">MEDIUM<td id="td1">AMOUNT<td id="td2">UNIT<td id="td3">DETAILS</tr>
          <tr><td>energy usage</td><td id="p01"></td><td>kWh</td><td><div class='popup'><button class='btn' id='btn1'>details</button><span class='popuptext' id='myPopup1'></span><div></td></tr>
          <tr><td>energy return</td><td id="p11"></td><td>kWh</td><td><div class='popup'><button class='btn' id='btn2'>details</button><span class='popuptext' id='myPopup2'></span><div></td></tr>
          <tr><td>power ph 1</td><td id="p21"></td><td>W</td><td>
          <tr class='phase-row'><td>power ph 2</td><td id="p22"></td><td>W</td>
          <tr class='phase-row'><td>power ph 3</td><td id="p23"></td><td>W</td></tr>
          <tr><td>gas consumed</td><td id="p31"></td><td>m3</td><td id="p30"></tr>
        </table>
        </div>  
        <p>Powered by Hansiart</p>
             
        </center>
    </div>
<script type="text/javascript" src="JAVASCRIPT"></script>
</div></body>

</html>
)=====";

const char JAVA_SCRIPT[] PROGMEM = R"=====(

var btn1 = document.getElementById('btn1');
var btn2 = document.getElementById('btn2');
var popup = document.getElementById("myPopup");
console.log("btn1 = " + btn1);
var CON_HT;
var CON_LT;
var RET_HT;
var RET_LT;
var PWRC1;
var PWRR1;
var PWRC2;
var PWRR2;
var PWRC3;
var PWRR3;

function loadScript() {
 init() 
 getData();
}

function init() {
  btn1.addEventListener('click', pop1);
  btn2.addEventListener('click', pop2);
  // Luister ALTIJD naar kliks op de achtergrond om te sluiten
  document.addEventListener('click', hidePopup);
}

function pop1(e) {
  // Voorkom dat deze klik direct hidePopup activeert
  if (e) e.stopPropagation();

  // 1. Verberg altijd direct popup 2
  var popup2 = document.getElementById("myPopup2");
  if (popup2) popup2.classList.remove("show");

  // 2. Open / toggle popup 1
  var popup = document.getElementById("myPopup1");
  popup.classList.toggle("show");
  popup.innerHTML = "ENERGY USAGE<br>  high tariff : " + CON_HT + " kWh<br>  low tariff: " + CON_LT + " kWh";
}

function pop2(e) {
  // Voorkom dat deze klik direct hidePopup activeert
  if (e) e.stopPropagation();

  // 1. Verberg altijd direct popup 1
  var popup1 = document.getElementById("myPopup1");
  if (popup1) popup1.classList.remove("show");

  // 2. Open / toggle popup 2
  var popup = document.getElementById("myPopup2");
  popup.classList.toggle("show");
  popup.innerHTML = "ENERGY RETURN<br>  high tariff : " + RET_HT + " kWh<br>  low tariff: " + RET_LT + " kWh";
}

function hidePopup(e) {
  // Als je op een button klikt, hoeft hidePopup niets te doen
  if (e.target.classList.contains('btn')) return;
  
  // Sluit allebei de popups zodra er op de achtergrond wordt geklikt
  var popup1 = document.getElementById("myPopup1");
  var popup2 = document.getElementById("myPopup2");
  
  if (popup1) popup1.classList.remove("show");
  if (popup2) popup2.classList.remove("show");
}


/*function init() {
  btn1.addEventListener('click', pop1);
  btn2.addEventListener('click', pop2);
  document.removeEventListener('click', hidePopup);
}
function change() {
  btn1.removeEventListener('click', pop1);
  btn2.removeEventListener('click', pop2);
  document.addEventListener('click', hidePopup);
}
function pop1() {
  var popup2 = document.getElementById("myPopup2");
  if (popup2) {
    popup2.classList.remove("show");
  }
  var popup = document.getElementById("myPopup1");
  popup.classList.toggle("show");
  popup.innerHTML="ENERGY USAGE<br>  high tariff : " + CON_HT + " kWh<br>  low tariff: " + CON_LT + " kWh";
  setTimeout(change,200);
}
function pop2() {
  var popup1 = document.getElementById("myPopup1");
  if (popup1) {
    popup1.classList.remove("show");
  }
  var popup = document.getElementById("myPopup2");
  popup.classList.toggle("show");
  popup.innerHTML="ENERGY RETURN<br>  high tariff : " + RET_HT + " kWh<br>  low tariff: " + RET_LT + " kWh";
  setTimeout(change,200);
}
*/
function hidePopup(a) {
  // Close both popups when there is clicked somewhere on the screen
  var popup1 = document.getElementById("myPopup1");
  var popup2 = document.getElementById("myPopup2");
  
  if (popup1) popup1.classList.remove("show");
  if (popup2) popup2.classList.remove("show");
   //popup.classList.toggle("show");
   document.removeEventListener('click', hidePopup);
   init();
   }

window.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'visible') {
       getData();
    }
});

function celbgc(cel) {
     document.getElementById(cel).style = "background-color:#c6ff1a";  
}
function celbga(cel) {
     document.getElementById(cel).style = "background-color:#f59b0a";  
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
//{"CON_HT":0,"CON_LT":51.775,"RET_HT":0,"RET_LT":24.413,"POWER_CON":0,"POWER_RET":0,"enR":24.413,"enC":51.775,"aPo":0,"gAs":16.713,"rm":0}
      CON_HT = obj.CON_HT;
      CON_LT = obj.CON_LT;
      RET_HT = obj.RET_HT;
      RET_LT = obj.RET_LT;
      PWRC1 =  obj.PWRC1; 
      PWRR1 =  obj.PWRR1;    
      PWRC2 =  obj.PWRC2; 
      PWRR2 =  obj.PWRR2;
      PWRC3 =  obj.PWRC3; 
      PWRR3 =  obj.PWRR3;      
      var enC = obj.enC;
      var enR = obj.enR;
      var aP1 = PWRC1 + PWRR2;
      var aP2 = PWRC2 + PWRR2;
      var aP3 = PWRC3 + PWRR3;
      //var gAa = obj.gAs;
      var rem = obj.rm;
      var threeP = obj.threeP;
      if(rem == 0) {document.getElementById("ml").style.display = "block";} // hide menu link         
      if(obj.threeP) {
           document.querySelectorAll('.phase-row').forEach(row => {
          row.style.display = 'table-row';
          });
      }
      cel="p01";
      if(enC < 0 ) celbga(cel); else celbgc(cel);
      if(obj.enC != "n/a") {
           document.getElementById(cel).innerHTML = enC.toFixed(3);
           } else {
           document.getElementById(cel).innerHTML = "n/a";
           } 

      cel="p11";
      if(enR < 0 ) celbga(cel); else celbgc(cel);
      if(obj.enR != "n/a") {
           document.getElementById(cel).innerHTML = enR.toFixed(3);
           } else {
           document.getElementById(cel).innerHTML = "n/a";
           }      

      cel="p21"; //pwr l1
      if(aP1 < 0 ) celbga(cel); else celbgc(cel);
      document.getElementById(cel).innerHTML = aP1;
           
      cel="p22"; //pwr l1
      if(aP2 < 0 ) celbga(cel); else celbgc(cel);
      document.getElementById(cel).innerHTML = aP2;
 
      cel="p23"; //pwr l1
      if(aP3 < 0 ) celbga(cel); else celbgc(cel);
      document.getElementById(cel).innerHTML = aP3;
             

      cel="p31";
      celbgc(cel);
      document.getElementById(cel).innerHTML = obj.gAs.toFixed(3);
 

      document.getElementById("noOutput").style.display = "none";

      document.getElementById("time").innerHTML = obj.timestamp;
      }
  }
  
  xhttp.open("GET", "/get.Data", true);
  xhttp.send();
}


if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
  if(e.data == "getall") {
  getData();
  }

 }, false);
}

)=====";
