#include <Arduino.h>

String setHtml(int tonm, int toff, int hon, int hoff, int tenab, int henab, int mrelay, int mobil, int levelbat, int bstatus, int bat_on, int bat_off) {
  String htmlout = R"(<!doctype html>
<html>
<head>
<meta http-equiv=Content-Type content="text/html; charset=utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP-01 Smart Relay</title>
<style>.buton{background-color:#04aa6d;border:none;color:#fff;padding:10px 20px}
.butoff{background-color:#f44336;border:none;padding:10px 20px;color:#fff}
.checkshow{display: none;}
.batshow{display: none;}
.imghow{
	visibility: hidden;
	display: none;
	}
</style>
</head>
<body id="bmain" onLoad=onload();>
<b>Управление реле в зависимости от</b><p>
 <input type="checkbox" name="chmob" id="chmob"><label for="chmob">Зарядка телефона</label><span id="param_bat"> </span>
<div class="batshow" id="setbatter"><p> 
Включение реле.&#32;<input id=batOn value=80 size=1>% <p>
Выключение реле.&#32;<input id=batOff value=90 size=1>% <p>
<input name=butbatSend type=button value=Сохранить onClick=saveBat()></p></div>
<div class="checkshow" id="chshow"><p> 
<select id=selectOn>
<option>Температура</option>
<option>Влажность</option>
</select>&#32;вкл./выкл.<input type=checkbox id=onoff_dev></p><p>
Включение реле.&#32;<input id=vOn value=20 size=1> <b id=pon>&#176C</b></p><p>
Выключение реле.&#32;<input id=vOff value=30 size=1> <b id=poff>&#176C</b></p><p>
<input name=butSend type=button value=Сохранить onClick=goTo()></p></div>
<p>Реле сейчас: <c id=srele>OFF</c></p>
<c id=img_off class=imghow><svg width=200.000 height=130.000 viewBox="0.0 0.0 200.000 130.000">
<path d="M16.84 49.63L83.23 49.63L83.23 82.83L16.84 82.83L16.84 49.63" fill=none stroke=black stroke-width=1.5 />
<path d="M50.03 16.44L50.03 49.63" fill=none stroke=black stroke-width=1.5 />
<path d="M50.03 82.83L50.03 116.02" fill=none stroke=black stroke-width=1.5 />
<path d="M83.23 66.23L91.53 66.23" fill=none stroke=black stroke-width=1.5 />
<path d="M99.83 66.23L108.13 66.23" fill=none stroke=black stroke-width=1.5 />
<path d="M116.43 66.23L124.73 66.23" fill=none stroke=black stroke-width=1.5 />
<path d="M133.02 116.02L133.02 91.13" fill=none stroke=black stroke-width=1.5 />
<path d="M133.02 91.13L149.62 41.33" fill=none stroke=black stroke-width=1.5 />
<path d="M133.02 49.63L152.94 49.63" fill=none stroke=black stroke-width=1.5 />
<path d="M133.02 49.63L133.02 16.44" fill=none stroke=black stroke-width=1.5 />
<path d="M166.22 49.63L166.22 16.44" fill=none stroke=black stroke-width=1.5 />
<path d="M133.02 66.23L141.32 66.23" fill=none stroke=black stroke-width=1.5 /></svg> </c>
<c id=img_on class=imghow><svg xmlns="http://www.w3.org/2000/svg" width="200" height="130">
<path fill="none" stroke="#000" stroke-width="1.5" d="M16.84 49.63h66.39v33.2H16.84v-33.2M50.03 16.44v33.19M50.03 82.83v33.19M83.23 66.23h8.3M99.83 66.23h8.3M116.43 66.23h8.3M133.02 116.02V91.13"/><path fill="none" stroke="#000" stroke-width="1.977" d="m132.898 92.582 31.757-45.221"/>
<path fill="none" stroke="#000" stroke-width="1.5" d="M133.02 49.63h19.92M133.02 49.63V16.44M166.22 49.63V16.44M133.02 66.23h8.3"/>
<path d="M17.604 66.275V50.502h64.84v31.545h-64.84Z" style="fill:#0f0;stroke-width:.159314"/>
</svg> </c>
<div id=showRelay> </div>
<input class=butoff type=button id=butOff value=OFF onClick=butOff()>&nbsp;&nbsp; &nbsp;<input class=buton type=button id=butOn value=ON onClick=butOn()><br>
<iframe  src="/param" id="mframe" class="imghow">
</iframe>
<script>
const tonm=)" + String(tonm) + R"(;
const toff=)" + String(toff)+ R"(;
const hon=)" + String(hon)+ R"(;
const hoff=)" + String(hoff) + R"(;
const tenab=)" + String(tenab)+ R"(;
const henab=)" + String(henab)+ R"(;
var relay=)" + String(mrelay)+ R"(;
const mobil=)" + String(mobil)+ R"(;
var levelbat=)" + String(levelbat)+ R"(;
var bstatus=)" + String(bstatus) + R"(;
const bat_on=)" + String(bat_on) + R"(;
const bat_off=)" + String(bat_off)+ R"(;
var indexon=0,imgon=document.getElementById("img_on"),imgoff=document.getElementById("img_off"),Onval=document.getElementById("vOn"),Offval=document.getElementById("vOff"),BatOn=document.getElementById("batOn"),BatOff=document.getElementById("batOff"),onoff_dev=document.getElementById("onoff_dev"),selectElemOn=document.getElementById("selectOn"),meteo=document.getElementById("chshow"),battery=document.getElementById("setbatter"),chmob=document.getElementById("chmob"),pon=document.getElementById("pon"),poff=document.getElementById("poff");function onload(){chmob.checked=Number(mobil),chmob.checked?(meteo.style.display="none",battery.style.display="block"):(meteo.style.display="block",battery.style.display="none"),Onval.value=tonm,Offval.value=toff,onoff_dev.checked=Number(tenab),BatOn.value=bat_on,BatOff.value=bat_off,setReleShow(relay),history.replaceState(null,"",location.pathname)}function bat_show(e,n){1==mobil&&(mbatstatus="  батарея: 🔋"+e+"%",1==n&&(mbatstatus+=" ⚡"),document.getElementById("param_bat").innerHTML=mbatstatus)}function setReleShow(e){1==e?(document.getElementById("showRelay").innerHTML=document.getElementById("img_on").innerHTML,document.getElementById("srele").innerHTML="ON"):(document.getElementById("showRelay").innerHTML=document.getElementById("img_off").innerHTML,document.getElementById("srele").innerHTML="OFF")}function goTo(){confirm("Сохранить значения в контроллере?")&&(murl=0==indexon?window.location.pathname+"?index="+indexon+"&to="+Onval.value+"&tf="+Offval.value+"&te="+onoff_dev.checked+"&endhead=1":window.location.pathname+"?index="+indexon+"&ho="+Onval.value+"&hf="+Offval.value+"&he="+onoff_dev.checked+"&endhead=1",window.location=murl)}function saveBat(){confirm("Сохранить значения в контроллере?")&&(murl=window.location.pathname+"?bat_on="+BatOn.value+"&bat_off="+BatOff.value+"&endhead=1",window.location=murl)}function butOff(){murl=window.location.pathname+"?index="+indexon+"&rele=0&endhead=1",window.location=murl}function butOn(){murl=window.location.pathname+"?index="+indexon+"&rele=1&endhead=1",window.location=murl}chmob.addEventListener("change",function(){murl=chmob.checked?(meteo.style.display="none",battery.style.display="block",window.location.pathname+"?index="+indexon+"&mobil=1&endhead=1"):(battery.style.display="none",meteo.style.display="block",window.location.pathname+"?index="+indexon+"&mobil=0&endhead=1"),window.location=murl}),selectElemOn.addEventListener("change",function(){0==(indexon=selectElemOn.selectedIndex)?(pon.innerHTML="&#176C",poff.innerHTML="&#176C",Onval.value=tonm,Offval.value=toff,onoff_dev.checked=Number(tenab)):(pon.innerHTML="%",poff.innerHTML="%",Onval.value=hon,Offval.value=hoff,onoff_dev.checked=Number(henab))}),setInterval(function(){try{document.getElementById("mframe").contentWindow.location.reload()}catch(e){}},5e3),document.getElementById("mframe").addEventListener("load",function(e){try{var n=this.contentWindow.document.body.innerHTML;index=0;var t=Array(7).fill(0);for(count=n.length;3<count;)pos=n.indexOf("<br>"),value=n.slice(0,pos),"nan"!=value?t[index]=value:t[index]=0,index++,n=(n=n.replace(value+"<br>","")).trim(),count=n.length;mlevelbat=t[3],mbstatus=t[4],mrelay=t[5]}catch(e){mlevelbat=0,mbstatus=0,mrelay=0}parent.setReleShow(mrelay),parent.bat_show(mlevelbat,mbstatus)});
</script></body></html>)";
  return htmlout;
}
