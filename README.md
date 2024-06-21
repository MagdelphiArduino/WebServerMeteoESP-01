
![](https://github.com/MagdelphiArduino/WebServerMeteoESP-01/blob/master/img/clock_meteo.gif)<br/>
<span style="font-size:1.3em;">A useful device based on an inexpensive relay control module via Wi-Fi.<br/>
The Medul with ESP01 broadcasts weather information via Wi-Fi and controls the relay depending on the parameters or controls Android charging using the Wall Clock application version 1.4 and older.</span><br/>
ESP01s relay module is used to communicate with pressure, temperature and humidity sensor.
![](https://github.com/MagdelphiArduino/WebServerMeteoESP-01/blob/master/img/s5.jpg) ![](https://github.com/MagdelphiArduino/WebServerMeteoESP-01/blob/master/img/s6.jpg)  ![](https://github.com/MagdelphiArduino/WebServerMeteoESP-01/blob/master/img/s7.jpg)The readings are displayed on the SSD1306 OLED display and transmitted via Wi-Fi. <br/>
For a stable connection, a static IP is set in the program.<br/>
The module relay is controlled depending on the set temperature and humidity values. <br/>
Configuring settings on any device with a browser at http://192.168.0.157/setting, displaying weather information http://192.168.0.157/index. The IP address is set in WebServerMeteoESP-01.ino, those who are familiar with Arduino will have no problems. <br/>
Android app "Wall Clock" every 10 seconds. sends a request with the battery status to http://192.168.0.157/param, having received a response, reads and displays weather information and, if charging control is enabled, controls charging via the ESP01 relay module.<br/>
Connection diagram.
![](https://github.com/MagdelphiArduino/WebServerMeteoESP-01/blob/master/img/s9.jpg)<br/>
<span style="font-size:1.3em;">[App on RuStore](https://www.rustore.ru/catalog/app/com.mag.time.clock)</span><br/> 
<span style="font-size:1.3em;">[Web site](https://magdelphi.ru/)</span><br/> 

[![](https://img.youtube.com/vi/pujb99hwVLg/0.jpg)](https://www.youtube.com/watch?v=pujb99hwVL)

Components:<br/>
Relay module ESP01s with Wi-Fi 5V [https://aliexpress.ru/item/32969212665.html](https://aliexpress.ru/item/32969212665.html) <br/>
OLED module IIC 0.96 inch, serial white display 128X64 I2C SSD1306 [https://aliexpress.ru/item/32830523451.html](https://aliexpress.ru/item/32830523451.html)<br/>
AHT20 + BMP280 High-precision digital temperature, humidity, atmospheric pressure sensor [https://aliexpress.ru/item/1005004517568566.html](https://aliexpress.ru/item/1005004517568566.html)

