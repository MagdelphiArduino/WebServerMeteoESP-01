
//#include <Arduino.h>
//#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <EEPROM.h>
#include <DNSServer.h>

int countScanRes = 0;
/* Установить свой SSID и пароль */
//const char* scan_ssid = "ESP82_Meteo";
//const char* scan_password = "01234567";
const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer scan_server(80);  //Captive Portal

////////////////////////////////////////////////////////////////////////////
String StcanForWiFis(byte flag) {
  if (flag > 0) WiFi.disconnect();  // disconnect from WiFi to enable promiscuitive WiFi-mode

  int numberOfNetworks = WiFi.scanNetworks();
  String htm = "";
  String rssi = "";
  String ssid = "";
  String encryp = " ";
  int rssu = 0;
  for (int i = 0; i < numberOfNetworks; i++) {
    ssid = WiFi.SSID(i);
    rssu = WiFi.RSSI(i);
    byte encryption = WiFi.encryptionType(i);

    if (rssu < -90) rssi = "q q-0";
    else if (rssu < -80) rssi = "q q-1";
    else if (rssu < -60) rssi = "q q-2";
    else if (rssu < -50) rssi = "q q-3";
    else if (rssu < -10) rssi = "q q-4";

    if (encryption != 7) encryp = " l";
    htm = htm + "<div><a href='#p' onclick='c(this)'>" + ssid + "</a><div role='img' aria-label='" + String(rssu) + "%' title='" + String(rssu) + "%' class='" + rssi + encryp + "'></div><div class='q'>" + WiFi.channel(i) + "</div></div>";
  }
  return htm;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void ScanForWiFis() {
  String rr = getSHtml(StcanForWiFis(1), modeWifi, sSSID, sPassword, sStaticIP, gatwei, pointSSID, pointPassword);
  scan_server.send(200, "text/html", rr);
  countScanRes = -120;
}


//////////////////////////////////////////////////////////////////////////////////////////
void scan_setup() {
  //Считываем переменные
  EEPROM.get(80, modeWifi);
  sSSID = readStringFromEEPROM(170);
  sPassword = readStringFromEEPROM(190);
  sStaticIP = readStringFromEEPROM(210);  //192.168.0.157
  gatwei = readStringFromEEPROM(230);     //192.168.0.1
  pointSSID = readStringFromEEPROM(250);
  pointPassword = readStringFromEEPROM(270);
  /* Настройка IP-адрес */
  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(127, 0, 0, 1);  // Primary DNS (optional)
  IPAddress secondaryDNS(8, 8, 8, 8);  // Secondary DNS (optional)

  WiFi.mode(WIFI_AP);
  delay(100);
  const char *qssid = pointSSID.c_str();          //"TP-Link_8C10";
  const char *qpassword = pointPassword.c_str();  //"beeline6867";

  WiFi.softAP(qssid, qpassword);
  delay(100);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  scan_server.on("/", ScanForWiFis);
  scan_server.on("/wifi", ScanForWiFis);
  scan_server.on("/wifisave", savePoint);

  dnsServer.setTTL(1);
  dnsServer.start(DNS_PORT, "*", local_ip);

  //вызывается, когда обработчик не назначен
  scan_server.onNotFound([]() {
    ScanForWiFis();
  });

  scan_server.begin();
  delay(100);
  countReset = 0;
  EEPROM.put(82, countReset);
  EEPROM.commit();
  delay(100);
  countScanRes = 0;
  //scan_server.send(200, "text/html", getStart(sStaticIP));  //ответить
}

//////////////////////////////////////////////////////////////////////////////////////////////
void scan_loop() {
  dnsServer.processNextRequest();
  scan_server.handleClient();
  delay(500);
  countScanRes++;
  if (countScanRes > 120) {
    delay(100);
    ESP.restart();  //перезагрузка модуля
    ESP.deepSleep(20);
    return;
  }
}


//записать данные выбранного роутера////////////////////////////////////////////////////
void savePoint() {
  String sid = scan_server.arg("s");
  String pas = scan_server.arg("p");
  String nStaticIP = scan_server.arg("ip");
  String ngatwei = scan_server.arg("g");

  writeStringToEEPROM(170, sid);
  writeStringToEEPROM(190, pas);
  writeStringToEEPROM(210, nStaticIP);
  writeStringToEEPROM(230, ngatwei);

  //Считываем переменные
  EEPROM.get(80, modeWifi);

  if (modeWifi == 0)
    scan_server.send(200, "text/html", getStart(nStaticIP));  //ответить
  else scan_server.send(200, "text/html", getStart("192.168.4.1"));

  delay(100);
  ESP.restart();  //перезагрузка модуля
  ESP.deepSleep(20);
}


////////////////////////////////////////////////////////////////////////////////
void writeStringToEEPROM(int addrOffset, const String strToWrite) {
  byte len = strToWrite.length();
  for (int i = 0; i < len; i++) {
    EEPROM.put(addrOffset + i, strToWrite[i]);
  }
  EEPROM.put(addrOffset + len, 0);
  EEPROM.put(addrOffset + len + 1, 0);
  EEPROM.commit();
}

/////////////////////////////////////////////////////////////////////////
String readStringFromEEPROM(int addrOffset) {
  String strout = "";
  int i = 0;
  byte chm = EEPROM.read(addrOffset + i);
  while (chm != 0) {
    i++;
    strout = strout + char(chm);
    chm = EEPROM.read(addrOffset + i);
  }
  return strout;
}
