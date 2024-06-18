
![](https://raw.githubusercontent.com/MagdelphiArduino/WebServerMeteoESP-01/master/img/clock_meteo.gif)<br/>
<span style="font-size:1.3em;">Полезное устройство на недорогом модуле управления реле по Wi-Fi.<br/>
Медуль с ESP01  транслирует по Wi-Fi метеоинформацию и управляет реле в зависимости от параметров или управляет зарядкой  Android  приложением "Настенные часы" версии 1.4 и старше.</span><br/>
Релейный модуль ESP01s  используется для связи с датчиком давления, температуры и влажности.
![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=img%2Fs5.jpg) ![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=img%2Fs6.jpg) ![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=img%2Fs7.jpg)
Показания выводятся на OLED дисплей SSD1306  и передаются по Wi-Fi . <br/>
Для стабильного соединения в программе задаётся статический IP.<br/>
Реле модуля управляется в зависимости от заданных значений температуры и влажности. <br/>
Настройка параметров на любом устройстве с браузером по адресу http://192.168.0.157/setting,  отображение метеоинформации http://192.168.0.157/index. IP адрес задаётся в WebServerMeteoESP-01.ino, кто знаком с Arduino проблем не бует. <br/>
Android  приложение "Настенные часы" каждые 10 сек. отсылает запрос с состоянием батареи на http://192.168.0.157/param, получив ответ считывает и выводит метеоинформацию и, если включено управление зарядкой,  управляет через релейный модуль ESP01 зарядкой.<br/>
Схема подключения.
![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=img%2Fs9.jpg)<br/>
<span style="font-size:1.3em;">[VIDEO работы модуля ESP01 с сенсором  AHT20 + BMP280 и OLED I2C дисплеем SSD1306 ](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=img%2Fv1.mp4)</span><br/> 
<span style="font-size:1.3em;">[Приложение на  RuStore ](https://www.rustore.ru/catalog/app/com.mag.time.clock)</span><br/> 
<span style="font-size:1.3em;">[Веб сайт](https://magdelphi.ru/)</span><br/> 
Компоненты:<br/>
Релейный модуль ESP01s с Wi-Fi 5В      [https://aliexpress.ru/item/32969212665.html](https://aliexpress.ru/item/32969212665.html) <br/> 
OLED-модуль IIC 0,96 дюйма, серийный белый дисплей 128X64 I2C SSD1306      [https://aliexpress.ru/item/32830523451.html](https://aliexpress.ru/item/32830523451.html)<br/>
AHT20 + BMP280 Высокоточный цифровой датчик температуры, влажности, атмосферного давления      [https://aliexpress.ru/item/1005004517568566.html](https://aliexpress.ru/item/1005004517568566.html)




