#include <Arduino.h>

String setHtml(int tonm, int toff, int hon, int hoff, int tenab, int henab, int mrelay, int mobil, int levelbat, int bstatus, int bat_on, int bat_off);
String getSHtml(String pointWiFi, byte modepoint, String ssid, String pasw, String ips, String gats, String sms, String pms);
String getStart(String stIP);
String getMeteo(String davl, String temper, String vlaz);
String getReset() ;


String hSrart = ""
                "<!DOCTYPE html>"
                "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'><title>ESP Start</title>"
                "<style>body{text-align:center;}div{text-align:center;display:inline-block;min-width:260px;}.mtable{width:100%;text-align:center;border-collapse:separate;border-radius:10px;border:1px solid #999;}.button{text-align:center;border:0;border-radius:.3rem;background-color:#1D92D3;color:#fff;line-height:2.4rem;font-size:20px;width:40%;}"
                "</style>"
                "<body onload=mLoad()><div class='button' onclick=mclear('settings')>Реле</div><p><div class='button' onclick=mclear('index')>Метео информация</div><p><div class='button' onclick=mclear('wifi')>Настройки WiFi</div><p><br>"
                "<div onclick=fclear('clear') class='button'>Сбросить настройки</div><p><div class='button' onclick=fclear('reset')>Перезагрузка</div>"
                "<script>  var path='';function mLoad(){murl=window.location.pathname,mpos=murl.indexOf('start'),path=murl.substring(1,mpos)}function mclear(n){window.location=path+n}function fclear(n){confirm('Выполнить?')&&(window.location=path+n)}"
                "</script>";