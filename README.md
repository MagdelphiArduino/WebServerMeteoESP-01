
![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01s/blob/raw?file=clock_meteo.gif)

Релейный модуль ESP01s  используется для связи с датчиком давления, температуры и влажности.
![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01/blob/raw?file=image%2Fs5.jpg) ![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01/blob/raw?file=image%2Fs6.jpg) ![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01/blob/raw?file=image%2Fs7.jpg)
Показания выводятся на OLED дисплей SSD1306  и передаются по Wi-Fi . 
Для стабильно соединения в программе задаётся статический IP.
Реле модуля управляется в зависимости от заданных значений температуры и влажности. 
Android  приложение "Wall Clock" выводит метеоинформацию и управляет через релейный модуль ESP01s зарядкой.

Схема подключения.
![](https://gitflic.ru/project/magdelphi/webservermeteoesp-01/blob/raw?file=image%2Fs9.jpg)

Компоненты:
Релейный модуль ESP01s с Wi-Fi 5В       https://aliexpress.ru/item/32969212665.html
OLED-модуль IIC 0,96 дюйма, серийный белый дисплей 128X64 I2C SSD1306      https://aliexpress.ru/item/32830523451.html
AHT20 + BMP280 Высокоточный цифровой датчик температуры, влажности, атмосферного давления      https://aliexpress.ru/item/1005004517568566.html