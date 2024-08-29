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
//const char* ssid = "";      //"TP-Link_8C10";
//const char* password = "";  //"beeline6867";

// Static IP configuration
IPAddress staticIP(192, 168, 0, 157);  // ESP32 static IP задайте свои значения
IPAddress gateway(192, 168, 0, 1);     // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);    // Subnet mask
IPAddress primaryDNS(192, 168, 0, 1);  // Primary DNS (optional)
IPAddress secondaryDNS(8, 8, 8, 8);    // Secondary DNS (optional)

Adafruit_BMP280 bme;  //https://aliexpress.ru/item/1005004517568566.html
AHT20 myAHT20(0x38);

WiFiServer server(80);                 // Указываем, что сервер будет использовать 80 порт
String header;                         // Переменная для хранения HTTP запроса
unsigned long currentTime = millis();  // Текущее время
unsigned long previousTime = 0;        // Переменная для сохранения времени подключения пользователя
const long timeoutTime = 10000;        // Определяем задержку в миллисекундах
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
int start_point = 0;
boolean mods = true;  //режим точка доступа - сервер
byte modeWifi = 0;    //режим WiFi
byte countReset = 0;
String sSSID;          // = "";
String sPassword;      // = "";
String sStaticIP;      // = "";
String gatwei;         // = "";
String pointSSID;      // = "";
String pointPassword;  // = "";

char* IPAddress2String(IPAddress& ip) {
  static char str_IP[16];
  char* last = str_IP;
  for (int8_t i = 0; i < 4; i++) {
    itoa(ip[i], last, 10);
    last = last + strlen(last);
    if (i == 3) *last = '\0';
    else *last++ = '.';
  }
  return str_IP;
}

void setup() {
  ESP.eraseConfig();
  EEPROM.begin(512);  // инициализация 512 байт EEPROM памяти
  EEPROM.get(498, startapp);
  if (startapp != 176) {  //запись начальных значений 1 раз
    EEPROM.put(498, 176);
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
    EEPROM.put(170, 0);
    EEPROM.put(80, 1);  //modeWifi
    EEPROM.put(82, 0);  //countReset
    EEPROM.commit();
    //   delay(20);
    writeStringToEEPROM(170, "");
    writeStringToEEPROM(190, "");
    writeStringToEEPROM(210, "192.168.0.157");
    writeStringToEEPROM(230, "192.168.0.1");
    writeStringToEEPROM(250, "ESP82_Meteo");
    writeStringToEEPROM(270, "01234567");
  }

  //Считываем переменные
  EEPROM.get(80, modeWifi);
  EEPROM.get(82, countReset);
  sSSID = readStringFromEEPROM(170);
  sPassword = readStringFromEEPROM(190);
  sStaticIP = readStringFromEEPROM(210);  //192.168.0.157
  gatwei = readStringFromEEPROM(230);     //192.168.0.1
  pointSSID = readStringFromEEPROM(250);
  pointPassword = readStringFromEEPROM(270);
  // delay(20);
  staticIP.fromString(sStaticIP);
  // delay(20);
  gateway.fromString(gatwei);
  //  delay(20);

  // Serial.begin(115200);
  // Serial.println("считываем переменные");

  if (sSSID.equals("")) {
    mods = false;
    scan_setup();
    return;
  } else {
    mods = true;

    WiFi.softAPdisconnect();
   WiFi.setSleepMode(WIFI_NONE_SLEEP,0);
   WiFi.setOutputPower(20.5);
    if (modeWifi == 0) {
      WiFi.mode(WIFI_STA);  //WIFI_AP_STA,  WIFI_STA
    } else {
      WiFi.mode(WIFI_AP_STA);
      IPAddress local_ip(192, 168, 4, 1);
      IPAddress gateway(192, 168, 4, 1);
      IPAddress subnet(255, 255, 255, 0);

      const char* qssid = pointSSID.c_str();
      const char* qpassword = pointPassword.c_str();

      WiFi.softAP(qssid, qpassword, 3, 0, 8);

      WiFi.softAPConfig(local_ip, gateway, subnet);
    }

    // Configuring static IP
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
      printString("Failed to configure Static IP", 1, 1);
    }

       ////////////////////////////////////////////////////////////////////////////////
    Wire.setClock(WireClock);  // Разгоняем шину I2C до 400 кгц
    Wire.begin(swSDA, swSCL);  //определяем выводы I2C

    oled_init();  // I2C SSD1306 https://aliexpress.ru/item/32957309383.html
    // delay(20);
    printString(F("МЕТЕО ESP-01s"), 1, 1);
    printString("SMART charg", 1, 3);
    printString("WIFI connecting", 1, 5);

    WiFi.begin(sSSID.c_str(), sPassword.c_str());

   int count = 0;
    //  delay(20);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      count++;
      if (count > 40) {
        countReset++;  //если WIFI_STA и 20с
        if (countReset < 3) {
          EEPROM.put(82, countReset);
          EEPROM.commit();
          delay(100);
          ESP.restart();  //перезагрузка модуля
          ESP.deepSleep(20);
        } else {
          EEPROM.put(82, 0);
          EEPROM.commit();
          // delay(20);
          if (WiFi.getMode() == WIFI_STA) {
            WiFi.mode(WIFI_AP_STA);  //включение режима точки доступа 192.168.4.1
                                     // delay(100);
            mods = false;
            scan_setup();
            return;
          } else {
            break;
          }
        }
      }
    }
    // delay(20);
    server.begin();

    EEPROM.put(82, 0);
    EEPROM.commit();
    //считываем переменные
    EEPROM.get(10, tonm);
    EEPROM.get(15, toff);
    EEPROM.get(20, hon);
    EEPROM.get(25, hoff);
    EEPROM.get(30, tenab);
    EEPROM.get(35, henab);
    EEPROM.get(55, mobil);

    // Настраиваем выводы платы
    pinMode(RELAY, OUTPUT);
    EEPROM.get(40, valueRele);  //состояние реле
    if (valueRele == 0) digitalWrite(RELAY, HIGH);
    else digitalWrite(RELAY, LOW);

    count = 0;
    //подключение сенсоров /////////////////////////////////////////
    while (bme.begin() != true) {
      // Serial.println("BMP280 not connect");
      printString("BMP280 not connect", 1, 1);
      delay(500);
      count++;
      if (count > 5) break;
    }
    count = 0;
    while (myAHT20.begin() != true) {
      //  Serial.println("AHT20 not connected");
      printString("AHT20 not connected", 1, 1);
      delay(500);
      count++;
      if (count > 5) break;
    }

    clearDisplay();
  }
}


String getArg(String mheader, String arg) {
  String narg = arg + '=';
  int pos = mheader.indexOf(narg);
  if (pos >= 0) {
    int mpos = header.indexOf('&', pos + 2);
    if (mpos > 0) {
      String mh = header.substring(pos, mpos);
      int posv = mh.indexOf('=');
      String rdt = (mh.substring(posv + 1));
      rdt.trim();
      return rdt;
    } else {
      int kpos = header.indexOf("HTTP", pos);
      if (kpos > 0) {
        String kh = header.substring(pos, kpos - 1);
        int kposv = kh.indexOf('=');
        String ret = kh.substring(kposv + 1);
        ret.trim();
        return ret;
      }
    }
  }
  return "";
}

float temperature = 0;  //bme.readTemperature();  //считываем данные датчиков
float barPressure = 0;  // bme.readPressure();
float humidite = 0;     // myAHT20.getHumidity();
int counttemper = 0;

void loop() {
  if (!mods) {
    scan_loop();  //scanNat loop
  } else {
    //  malloc(10000);
    counttemper++;
    if (counttemper > 10) {
      counttemper = 0;
      temperature = bme.readTemperature();  //считываем данные датчиков
      barPressure = bme.readPressure();
      humidite = myAHT20.getHumidity();
    }

    EEPROM.get(55, mobil);  //состояние для мобильного приложения

    WiFiClient client = server.available();   // прослушка входящих клиентов
    if (client) {                             //если у Client нет данных, доступных для чтения, этот объект будет оценен как false
                                              // currentTime = millis();
                                              // previousTime = currentTime;
      String currentLine = "";                // создаем строку для хранения входящих данных
      while (client.connected()) {            //  && currentTime - previousTime <= timeoutTime выполняем программу, пока пользователь подключен
                                              // currentTime = millis();
        if (client.available()) {             // проверяем, есть ли входящее сообщение
          char c = client.read();             // читаем
          header += c;                        //добавляем в тело HTTP запроса
          if (c == '\n') {                    // если входящее сообщение – переход на новую строку (пустая строка)
            if (currentLine.length() == 0) {  // то считаем это концом HTTP запроса и выдаем ответ
                                              //
              // заголовок всегда начинается с ответа (например, HTTP/1.1 200 OK) добавляем тип файла ответа:
              client.println("HTTP/1.1 200 Ok\r\nServer:ESP\r\nCache-Control:no-cache\r\nContent-Type:text/html\r\nConnection:close\r\n\r\n");
              //  client.println("");
              // client.println("");
              // client.println("HTTP/1.1 200 OK Content-type:text/html");
              // client.println();

              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              if (header.indexOf("GET /wifisave") >= 0) {
                sSSID = getArg(header, "s");
                writeStringToEEPROM(170, sSSID);
                sPassword = getArg(header, "p");
                writeStringToEEPROM(190, sPassword);
                sStaticIP = getArg(header, "ip");
                writeStringToEEPROM(210, sStaticIP);
                gatwei = getArg(header, "g");
                writeStringToEEPROM(230, gatwei);

                client.println(getStart(sStaticIP));  //вывод сообщения об успешной записи в контроллер, задержка 10 сек  и переход на http://ip/start
                staticIP.fromString(sStaticIP);       //меняем IP
                gateway.fromString(gatwei);

                header = "";
                client.stop();

                delay(100);
                ESP.restart();  //перезагрузка модуля
                ESP.deepSleep(20);
                return;
              }


              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              if (header.indexOf("GET /espsave") >= 0) {
                client.println(getStart(IPAddress2String(staticIP)));  //вывод сообщения об успешной записи в контроллер, задержка 10 сек  и переход на http://ip/start
                String cp = getArg(header, "cp");
                if (cp.equals("on")) {
                  EEPROM.put(80, 1);  //modeWifi
                } else {
                  EEPROM.put(80, 0);  //modeWifi}
                }
                EEPROM.commit();
                delay(20);

                pointSSID = getArg(header, "sm");
                writeStringToEEPROM(250, pointSSID);
                pointPassword = getArg(header, "pm");
                writeStringToEEPROM(270, pointPassword);

                header = "";
                client.stop();

                delay(100);

                ESP.restart();  //перезагрузка модуля
                ESP.deepSleep(20);
                return;
              }


              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              if (header.indexOf("GET /start") >= 0) {
                client.println(hSrart);
                client.println("<br><p>");
                client.println("Свободной памяти " + String(ESP.getFreeHeap()) + " байт");
                header = "";
                client.println();
                client.stop();
                //  delay(10);
                return;
              }


              //проверяем тело HTTP запроса на вхождение
              if (header.indexOf("GET /clear") >= 0) {  //сброс значений
                writeStringToEEPROM(170, "");
                writeStringToEEPROM(190, "");
                writeStringToEEPROM(210, "192.168.0.157");
                writeStringToEEPROM(230, "192.168.0.1");
                writeStringToEEPROM(250, "ESP82_Meteo");
                writeStringToEEPROM(270, "01234567");
                client.println("Clear WiFi settings");
                String text = "Connect to the controller access point (default \"ESP82_Meteo\" password \"01234567\") the browser will open (IP 192.168.4.1) and configure the connection";
                client.println(text);
                delay(10);
                client.stop();

                delay(100);
                ESP.restart();  //перезагрузка модуля
                ESP.deepSleep(10);
                return;
              }

              if (header.indexOf("GET /reset") >= 0) {
                client.println(getReset());
                header = "";
                delay(10);
                client.stop();

                delay(100);
                ESP.restart();  //перезагрузка модуля
                ESP.deepSleep(10);
                return;
              }

              if (header.indexOf("GET /wifi") >= 0) {
                //  String rr =
                //String rr = getSHtml(StcanForWiFis(0));  //без дисконекта

                client.println(getSHtml(StcanForWiFis(0), modeWifi, sSSID, sPassword, sStaticIP, gatwei, pointSSID, pointPassword));
                header = "";  // Очищаем заголовок  <iframe src="/param"></iframe>
                client.println();
                client.stop();

                return;
              }

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


              if (header.indexOf("GET /setting") >= 0) {
                int pos = header.indexOf("?");
                if (pos >= 0) {
                  int mpos = header.indexOf("endhead");
                  String mh = header.substring(pos + 1, mpos + 2);

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
                  delay(10);
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
                // delay(10);
                client.println(setHtml(tonm, toff, hon, hoff, tenab, henab, valueRele, mobil, levelbat, bstatus, bat_on, bat_off));
                client.println("<p><br><a href='/start'> Выйти</a>");
                header = "";
                client.println();
                //  client.println("Свободной памяти " + String(ESP.getFreeHeap()) + " байт");
                client.stop();
                //  delay(10);
                return;
              }

              if (header.indexOf("GET /index") >= 0) {  //страница с метео данными
                client.println(getMeteo(String(barPressure / 133.3F), String(temperature), String(humidite)));
                header = "";
                client.println();
                client.stop();
                // delay(10);
                return;
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

      if ((int)levelbat == 0) {  // на второе устройство не реагировать
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
      delay(20);
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
}