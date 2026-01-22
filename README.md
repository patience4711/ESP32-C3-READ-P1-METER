# ESP32-C3-READ-P1-METER

# ESP-READ-P1-METER

The purpose of this project is to read data from a so called smart meter (model Sagecom 210 ESMR5 or other) via its serial port. All we have to do is connect an ESP32C3 device (tested with ESP32-C3) via an rj11 cable to the serial port of the meter. The data is made available via mosquitto or an API and is also displayed on a webpage. <br>
Now we can process the data in our domotica systems like 'Domotics' to display graphs, control switches or manage a homebattery.<br>The API is compatible with the p1 dongle from HomeWizzard. To get data, the api is http://ip_of_p1meter/api/v1/data. If you want simple data f.i. for a solar diverter: http://ip_of_p1meter/api/v1/simple 

![frontpage](https://github.com/patience4711/ESP-READ-P1-METER/assets/12282915/bb65cf1f-f6bf-4e1c-ae48-c379628f3a7a)<br>

I know this has been done before but not on this platform. Moreover this project has more features. The esp32c3 can be fed with the 5v of the meter. 

The program has a lot of smart features. All settings can be done via the webinterface. Because the ESP has only one reliable working hardware serial port, this port is dedicated to the serial communication with the p1 meter. For the debugging we can use a web console just like in my other projects where the serial port is dedicated to the zigbee module. In the console we can call some processes and watch the output. 
See the [WIKI](https://github.com/patience4711/ESP-READ-P1-METER/wiki/GENERAL) for information on building it, the working, etc. 

## status and todo's
The software has been tested on a Sagemcom T210 meter and works good.
In the system we can experiment with several settings to make it work with other meters. In general the software is capable of reading the telegrams of other meters if they have an "open collector" port. The eventual signal inversion can be set in the ui so no signal inverter is needed.

Since the esp32c3 has bluetooth capablities, i can invest how to make use of that for this project.

## links
Here are some links to the projects where i got my inspiration (thanks to all for the good work they did.)
 * http://domoticx.com/arduino-p1-poort-telegrammen-uitlezen/
 * http://domoticx.com/p1-poort-slimme-meter-hardware/
 * https://github.com/romix123/P1-wifi-gateway/blob/main/src/P1WG2022current.ino
 * http://www.gejanssen.com/howto/Slimme-meter-uitlezen/#mozTocId935754

## compile this sketch
You can use the provided binary but if you must compile it yourself: Use arduino ide with the esp822 version 2.7.1 installed under boardmanager. The ota updates won't work with other versons.
<br>Please note: by popular demand I have published the code here but i do not feel responsible for problems as to compiling. Impossible for me to know why it doesn't compile in your situation.

## downloads
januari 2026: There is a version for ESP32C3 available.<br> 
Download [ESP-P1METER-v3_](https://github.com/patience4711/ESP32-C3-READ-P1-METER/blob/main/ESP32C3_P1METER_v0_3.ino.bin)<br>

## features
- compatible with home-wizzard p1 dongle
- simply to connect to your wifi
- automatic polling or on demand via mqtt or http
- data can be requested via http and mosquitto
- data is displayed on the frontpage, as a monthly report.
- we can see the telegram sent by the meter.
- Fast asyc webserver.
- a very smart on-line console to send commands and debugging.
- Smart timekeeping.
- A lot of system info on the webpage.
  

## the hardware
It is nothing more than an esp8266 device (tested with nodeMcu. The other materials are
- a prepared cable with an 6-pins RJ-11 plug.
- a 4K7 resistor to pullup the RX pin on the meter.
For info on how to build and use it, please see the <a href=''>WIKI</a>

## how does it work
The P1-meter spits out data (a so called telegram) when its rx pin is pulled high. Since we need a pullup resistor for the open collector in tx, this resistor is also connected with rx. So when we trigger the meter, this tx pin is also pulled up with 3,3 volt, this means no troubles with the 5v intolerance of an esp input.  We now receive the telegram, this has the form of a textdocument containing lines that each represent a value.
It starts with a "/" and ends with a "!". 

The program makes rx high and pulls-up the tx, next reads the serial port until the "/" is found. Now the following incoming bytes are stored in a char array until the endcharacter "!" is encountered. So now we have the full telegram as a char array.
Next the checksum calculation is done and when the char array is approved, the interesting values can be extracted.

## changelog ##
version ESP8266-P1METER-v0_3:
  - made compatible with home-wizzard
  - made compatible with other meters
  - we can edit monthly values now
version ESP8266-P1METER-v0_2:
  - cosmetical improvements

 
