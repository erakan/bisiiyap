# bi\$iiyap
bi\$iiyap is a Winamp plug-in that does something on everytime played music change.

## Description

I wanted to share the song names on my blog that I was listening on Winamp while I was working. So I created this project around 2004 to share my music files names in my blog. I olso created a script for my web site that accepts this inspormation and list the music names on my blog. This is an old project of mine and it has comments and descriptions mostly in Turkish language.


## License

This project is licensed under the GNU General Public License - see the LICENSE.md file for details 

---

# Old Turkish Readme.txt

## Nasil?
"$ukela" bölümünde, "$unu yap"dan istediğiniz bir olayi seçin. "$oyle yap" kisminda bir-iki kutu belirir.

- **"bi url gonder" icin:** url kismina sitenizdeki, submit edilecek formu yazin. ornegin: 
``` http://www.benimsitem.com/submitmp3.asp?sarki='ARTIST' - 'SARKI' ```

- **"sonucunu buna yaz"** kismina ise islemin sonucunun tutulacagi kayit dosyasinin adini verin. ornegin: ``c:\zart_submit.txt``

- **"bi$ii çalıştır" icin:** komut kismina hazirladiğiniz BAT, EXE vs. programlari yazin. Burada tikkat edilecek nokta $udur ki, bu BAT dosyalara $arki, Artist ismi vs. parametre olarak verilecektir. ornegin:
```c:\ftpilegonder.bat 'ARTIST' 'SARKI'```

### Kullanabilecegimiz kisaltmalar neler?
***Tikkat, butun kisaltmalar apostrof (') icinde yazilmalidir.***

- **'SARKI'** yazin Sarkinin adi gelsin.
- **'ARTIST'** yazin sarkiyi soyleyenin adi gelsin.
- **'ALBUM'** yazin albumun ismi gelsin.<sub>*</sub>
- **'DOSYA'** yazin dosyanin tam adresi gelsin.

<sub>*</sub> 'ALBUM' kismi sadece "ID3 bilgisini girc girc" aktif iken calisir.

## Nedir?

Bu plug-in'in fikir babası Ed Zaleski (oddsock@oddsock.org)'dir. 2000 yılında, DoSomething olarak bu arkadaş tarafından yazılan kodlar baz alınarak hazırlanmıştır. DoSomething bir işe yaramadığı için tarafımdan, Erdinç AKAN (erakan@gmail.com) tekrar yazılmıştır.

## Neden?
Yeni web siteme "$u anda $u şarkıyı dinliyorum" gibi artistik hareketler yerlestirdim. Tabii bunu yapmak iki aşamalı bir iş. Birincisi web sitesini ayarlamak, ikincisi Winamp ile bir plug'in çalıştırmak. Birinci kısım sorun değildi. 10-15 satırlık bir ASP programı ile halledildi. Winamp kısmı ise, biraz kastırtacaktı. Benimde hiç kasmaya niyetim olmadığı için aldım elime Google'i başladım Winamp ve Blog kelimelerini aramaya. Derken karşıma Ed'in yazdığı plug-in çıktı. Sitesinde kaynak kodları da dağıtıyordu. Programlama ile uğraşmaya hiç niyetim olmadığı için direkt binary'lerini indirdim kurdum. Tabii çalışmadı. Saçmaladı durdu. Dedim başka plug-in'ler arayayım onları kullanayım. Bir iki tane daha buldum ama onlar da benim istediğimi yapmıyorlardı. Sonuçta iş başa düştü. Ed'in kodlarını indirip, neyi nasıl yaptığına baktıktan sonra boş bir cumartesi akşamı bu programı kodlamaya başladım. Ertesi güne şu anda kullandığınız sürüm hazırdı.

## Niye ki?
Çünkü plug-in'in aslı, URL gönderirken sapıtıyordu, zart-zurt çöküyordu, düzeltmek gerekiyordu. Ed'in yazdığında XML, HTML oluşturma, FTP'den dosya upload etme gibi bi yiğin ıvır zıfır vardı. Bunları yazmadım. Bana sadece URL submit yeterliydi. Fakat başka arkadaşların değişik eyilimleri olabileceği aklıma geldiği için komut çalıştırmayı yaptim. Lakin hem XML-HTML, hem de FTP upload işini BAT dosyalarla, oda olmadi yazacaginiz programlarla çok kolay yapabilenzi değil mi cevat ağbi? (evet)

## Nereden?
Bu plug-in'i GNU General Public License altında open source olarak dagitiyorum. Hemen artizlik yapip adamin kaynak kodlari calmis biraz kurcalamis yapmis demeyin, lakin adamin kodlari felaket, isteyen olursa diye kendi sitemden diger kodlari da dagitiyorum. XML isteyen bu kodlari kurcalayabilir (Allah sabir vere ;) ). Bunun disinda kodlarin %90'i degisti. Web sitemden daha ayrintili bilgi alabilir. ASP ile beraber bu olay nasil yapiliyor ogrenebilirsiniz.

web: http://erdincakan.com
mayil: erakan@gmail.com

## Hak hukuk cart curt?
Bu plug-in size olduğu gibi verilmektedir. Ekinde kaynak kodları da vardır. Bu plug-in'i sisteminize kurmamalısınız. Kullanmamalısınız. Ben bu plug-in'in ne olduğunu bilmiyorum. Ben kimimim, siz kimsiniz bilmiyoum. Sizin bilgisayarınızı bozan program bu değil. Ben görmedim, duymadım hakim bey. Ben ne bilem?

## Aaa şunu da eklesene?
Sen eklesene?!!

## Gelecek?
Programda ID3v1 okuma var, bunun yanına ID3v2 eklenebilir de structure'u yok elimde. Bir arkadaş temin ederse mintaka eklerim ;)

## Kodlar?

Paul DiLascia'nın dediği gibi: "Kodlar çalışıyorsa, Erdinç AKAN erakan@gmail.com tarafından yazılmıştır. Çalışmıyorsa kimin yazdığını bilmiyorum!.."

## Son sözleriniz?
Kırmızı teldi, evet evet, bide onu keseyim bakalım...
