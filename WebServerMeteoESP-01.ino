/*
ESP-01s meteo informer for Android app "Wall Clock" V1.4 & show  I2C SSD1306
2024 06      https://magdelphi.ru/
ESP-01s   https://aliexpress.ru/item/32824783166.html
box  https://aliexpress.ru/item/1005004052734295.html
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <AHT20.h>
#include <EEPROM.h>
#include "display.h"
#include "esp32_Smart.h"

// Замените своими данными
const char* ssid = "Ваш роутер";
const char* password = "Ваш пароль";
/////////////////////////////////

// Static IP configuration
IPAddress staticIP(192, 168, 0, 157);  // ESP32 static IP задайте свои значения
IPAddress gateway(192, 168, 0, 1);     // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);    // Subnet mask
IPAddress primaryDNS(192, 168, 0, 1);  // Primary DNS (optional)
IPAddress secondaryDNS(0, 0, 0, 0);    // Secondary DNS (optional)

Adafruit_BMP280 bme;  //https://aliexpress.ru/item/1005004517568566.html
AHT20 myAHT20(0x38);

WiFiServer server(80);                 // Указываем, что сервер будет использовать 80 порт
String header;                         // Переменная для хранения HTTP запроса
unsigned long currentTime = millis();  // Текущее время
unsigned long previousTime = 0;        // Переменная для сохранения времени подключения пользователя
const long timeoutTime = 1000;         // Определяем задержку в миллисекундах
int countloop = 0;                     //для задержки
int tonm;
int toff;
int hon;
int hoff;
int tenab = 0;
int henab = 0;
int valueRele = 0;
int mobil = 0;
int levelbat = 0;
int bstatus = 0;
int bat_on = 80;
int bat_off = 90;
int startapp = 0;

void setup() {
  EEPROM.begin(512);  // инициализация 512 байт EEPROM памяти

  EEPROM.get(240, startapp);
  if (startapp != 176) {  //запись начальных значений 1 раз
    EEPROM.put(240, 176);
    EEPROM.put(10, 20);
    EEPROM.put(15, 26);
    EEPROM.put(20, 22);
    EEPROM.put(25, 35);
    EEPROM.put(30, 0);
    EEPROM.put(35, 0);
    EEPROM.put(40, 0);
    EEPROM.put(55, 0);
    EEPROM.put(60, 0);
    EEPROM.put(65, 0);
    EEPROM.put(70, 80);
    EEPROM.put(75, 90);
    EEPROM.commit();
  }

  // Настраиваем выводы платы
  pinMode(RELAY, OUTPUT);

  EEPROM.get(40, valueRele);

  if (valueRele == 0) digitalWrite(RELAY, HIGH);
  else
    digitalWrite(RELAY, LOW);

  // Serial.begin(115200);
  Wire.setClock(WireClock);  // Разгоняем шину I2C
  Wire.begin(swSDA, swSCL);

  oled_init();  // I2C SSD1306 https://aliexpress.ru/item/32957309383.html

  //подключение сенсоров
  while (bme.begin() != true) {
    printString("BMP280 not connect", 1, 1);
    delay(500);
  }
  while (myAHT20.begin() != true) {
    printString("AHT20 not connected", 1, 1);
    delay(500);
  }


  // Configuring static IP
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    printString("Failed to configure Static IP", 1, 1);
  }


  printString(F("МЕТЕО ESP-01s"), 1, 1);
  printString("SMART charg", 1, 3);
  printString("WIFI connecting", 1, 5);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.begin();

  //считываем переменные
  EEPROM.get(10, tonm);
  EEPROM.get(15, toff);
  EEPROM.get(20, hon);
  EEPROM.get(25, hoff);
  EEPROM.get(30, tenab);
  EEPROM.get(35, henab);
  EEPROM.get(55, mobil);

  clearDisplay();
}



void loop() {

  float temperature = bme.readTemperature();  //считываем данные датчиков
  float barPressure = bme.readPressure();
  float humidite = myAHT20.getHumidity();

  // Serial.println(relay);
  EEPROM.get(55, mobil);

  WiFiClient client = server.available();  // прослушка входящих клиентов

  if (client) {  //Если подключается новый клиент,Если есть подключение,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                                                   // создаем строку для хранения входящих данных
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // выполняем программу, пока пользователь подключен
      currentTime = millis();
      if (client.available()) {  // проверяем, есть ли входящее сообщение
        char c = client.read();  // читаем
        header += c;
        if (c == '\n') {  // если входящее сообщение – переход на новую строку (пустая строка)
          // то считаем это концом HTTP запроса и выдаем ответ
          if (currentLine.length() == 0) {
            // заголовок всегда начинается с ответа (например, HTTP/1.1 200 OK)
            // добавляем тип файла ответа:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /powon") >= 0) {  //включить реле
              digitalWrite(RELAY, LOW);
              valueRele = 1;
              EEPROM.put(40, valueRele);
              EEPROM.commit();
            } else if (header.indexOf("GET /powoff") >= 0) {  //выключить реле
              digitalWrite(RELAY, HIGH);
              valueRele = 0;
              EEPROM.put(40, valueRele);
              EEPROM.commit();
            }

            if (header.indexOf("GET /param") >= 0) {  // данные для Андроид приложения
              client.println(temperature);
              client.println("<br/>");
              client.println(barPressure);
              client.println("<br/>");
              client.println(humidite);
              client.println("<br/>");
              client.println(levelbat);
              client.println("<br/>");
              client.println(bstatus);
              client.println("<br/>");
              client.println(valueRele);
              client.println("<br/>");
              client.println(255);
              client.println("<br/>");
              header = "";
            }

            if (header.indexOf("GET /reset") >= 0) {
              client.println("ESP-01 Reset");
              delay(10);
              client.stop();
              delay(10);
              ESP.restart();  //перезагрузка модуля
              ESP.deepSleep(20);
              return;
            }


            if (header.indexOf("GET /setting") >= 0) {
              int pos = header.indexOf("?");
              if (pos >= 0) {
                int mpos = header.indexOf("endhead");
                String mh = header.substring(pos + 1, mpos + 2);
                //  Serial.println(mh);

                while (mh.length() > 5) {
                  int istart = mh.indexOf("=");
                  int iend = mh.indexOf("&");
                  String sparam = mh.substring(istart + 1, iend);
                  int param = sparam.toInt();
                  String name = mh.substring(0, istart);
                  mh.remove(0, iend + 1);
                  if (name.equals("to")) {
                    tonm = param;
                    EEPROM.put(10, tonm);
                  }
                  if (name.equals("tf")) toff = param;
                  EEPROM.put(15, toff);
                  if (name.equals("te")) {
                    if (sparam.equals("true")) tenab = 1;
                    else tenab = 0;
                    EEPROM.put(30, tenab);
                  }

                  if (name.equals("ho")) {
                    hon = param;
                    EEPROM.put(20, hon);
                  }
                  if (name.equals("hf")) {
                    hoff = param;
                    EEPROM.put(25, hoff);
                  }
                  if (name.equals("he")) {
                    if (sparam.equals("true")) henab = 1;
                    else henab = 0;
                    EEPROM.put(35, henab);
                  }

                  if (name.equals("rele")) {
                    if (param == 0) digitalWrite(RELAY, 1);
                    else digitalWrite(RELAY, 0);
                    valueRele = param;
                    EEPROM.put(40, valueRele);
                  }

                  if (name.equals("mobil")) {
                    mobil = param;
                    EEPROM.put(55, mobil);
                  }

                  if (name.equals("levbat")) {
                    levelbat = param;
                    EEPROM.put(60, levelbat);
                  }

                  if (name.equals("bstatus")) {
                    bstatus = param;
                    EEPROM.put(65, bstatus);
                  }


                  if (name.equals("bat_on")) {
                    bat_on = param;
                    EEPROM.put(70, bat_on);
                  }
                  if (name.equals("bat_off")) {
                    bat_off = param;
                    EEPROM.put(75, bat_off);
                  }
                }
                header = "";
                EEPROM.commit();
              }
              //считываем переменные
              EEPROM.get(10, tonm);
              EEPROM.get(15, toff);
              EEPROM.get(20, hon);
              EEPROM.get(25, hoff);
              EEPROM.get(30, tenab);
              EEPROM.get(35, henab);
              EEPROM.get(40, valueRele);
              EEPROM.get(55, mobil);
              EEPROM.get(60, levelbat);
              EEPROM.get(65, bstatus);
              EEPROM.get(70, bat_on);
              EEPROM.get(75, bat_off);
              client.println(setHtml(tonm, toff, hon, hoff, tenab, henab, valueRele, mobil, levelbat, bstatus, bat_on, bat_off));
              header = "";
            }

            if (header.indexOf("GET /index") >= 0) {  //страница с метео данными
              // Выводим HTML-страницу
              client.println("<!DOCTYPE html><html><head><meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"><meta content=\"width=device-width, initial-scale=1\" name=\"viewport\"/><link rel=\"icon\" href=\"data:\"><meta charset=\"utf-8\"><style>body {text-align: center;font-family: \"Times New Roman\", Arial;font-size: 24px;color: #30F;}.sensor {color: #900;");
              client.println("font-weight: bold;background-color: #bcbcbc;padding: 1px;font-family: \"Times New Roman\", Times, serif;font-size: 36px;}.temper {font-size: 50px;font-weight: bold;background-color: #bcbcbc;color: #900;}");
              client.println("</style></head><body bgcolor=\"#CCCCCC\"><h1>ESP32 с датчиком  BMP280</h1><br/><span class=\"temper\">Температура: ");
              client.println(temperature);
              client.println("&deg;C</span><br/><br/><span class=\"temper\">Давление:  ");
              client.println(barPressure / 133.3F);
              client.println("мм.рс</span><p><span class=\"temper\">Влажность: ");
              client.println(humidite);
              client.println("%</span><br/><br/></body></html>");
              header = "";
            }


            client.println();  // Ответ HTTP также заканчивается пустой строкой
            break;             // Прерываем выполнение программы
          } else {             // если у нас есть новый запрос, очищаем строку
            currentLine = "";
          }
        } else if (c != '\r') {  // но, если отправляемая строка не пустая
          currentLine += c;      // добавляем ее в конец строки
        }
      }
    }
    header = "";    // Очищаем заголовок  <iframe src="/param"></iframe>
    client.stop();  // Сбрасываем соединение
  }

  int newval = valueRele;
  int enab = 0;
  if (mobil == 0) {
    ///////////////////////Контроль показаний датчиков вкл/выкл реле ////////////////////////////////
    countloop++;
    if (countloop > 10) {  //раз в 3 сек.
      countloop = 0;

      //считываем переменные
      EEPROM.get(10, tonm);
      EEPROM.get(15, toff);
      EEPROM.get(20, hon);
      EEPROM.get(25, hoff);
      EEPROM.get(30, tenab);
      EEPROM.get(35, henab);
      EEPROM.get(40, valueRele);
      if (tenab > 0)  //проверка температуры включена
      {
        if ((int)temperature >= toff) {  //выключаем
          digitalWrite(RELAY, 1);
          valueRele = 0;
        };
        if ((int)temperature <= tonm) {  //включаем
          digitalWrite(RELAY, 0);
          valueRele = 1;
        };
      }
      if (henab > 0)  //проверка влажности включена
      {
        if ((int)humidite >= hoff) {  //выключаем
          digitalWrite(RELAY, 1);
          valueRele = 0;
        };
        if ((int)humidite <= hon) {  //включаем
          digitalWrite(RELAY, 0);
          valueRele = 1;
        };
      }
    }
  } else {  //управление зарядкой
    //считываем переменные
    EEPROM.get(60, levelbat);
    // EEPROM.get(65, bstatus);
    EEPROM.get(70, bat_on);
    EEPROM.get(75, bat_off);

    if ((int)levelbat == 0) {// на второе устройство не реагировать
      enab = 1;
    } else if ((int)levelbat >= bat_off) {  //выключаем
      digitalWrite(RELAY, 1);
      valueRele = 0;
    } else if ((int)levelbat <= bat_on) {  //включаем
      digitalWrite(RELAY, 0);
      valueRele = 1;
    }
  }

  String strrele = "ON ";

  uint32 pin_reg = GPIO_REG_READ(GPIO_OUT_ADDRESS);
  if ((pin_reg & 1) == HIGH) {
    strrele = "OFF ";
    valueRele = 0;
  } else {
    strrele = "ON  ";
    valueRele = 1;
  }

  if (newval != valueRele) {  //если реле переключено записываем
    EEPROM.put(40, valueRele);
    EEPROM.commit();
  }
  /////////////////Вывод на экран ///////////////////////////////////////////
  //printString(F("МЕТЕО ESP-01s"), 1, 1);
  printString("Влажн: " + String(humidite, 1) + " %", 1, 1);
  printString("Pатм: " + String(barPressure / 133.3F, 1) + " мм", 1, 3);
  printString("Tемп: " + String(temperature, 1) + " C", 1, 5);  //mobil,levelbat
  if (mobil == 0) printString("Реле " + strrele + "     ", 1, 7);
  else if (enab == 0) printString("Реле " + strrele + " " + String(levelbat) + "%", 1, 7);
 // else printString("Реле " + strrele + "     ", 1, 7);

  delay(300);
}