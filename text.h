#ifndef EA_ACIKLAMATEXTI_INCLUDED
#define EA_ACIKLAMATEXTI_INCLUDED

const char const *g_aciklamatexti = 
"Nasil?\n"
"\"$ukela\" bölümünde, \"$unu yap\"dan istediğiniz bir olayi seçin. \"$oyle yap\" kisminda bir-iki kutu belirir.\n"
"\n"
"\"bi url gonder\" icin:\n"
"\n"
"url kismina sitenizdeki, submit edilecek formu yazin. ornegin: http://www.benimsitem.com/submitmp3.asp?sarki='ARTIST' - 'SARKI'\n"
"\n"
"\"sonucunu buna yaz\" kismina ise islemin sonucunun tutulacagi kayit dosyasinin adini verin. ornegin: c:\\zart_submit.txt\n"
"\n"
"\"bi$ii çalı$tır\" icin:\n"
"\n"
"komut kismina hazirladiğiniz BAT, EXE vs. programlari yazin. Burada tikkat edilecek nokta $udur ki, bu BAT dosyalara $arki, Artist ismi vs. parametre olarak verilecektir. ornegin: c:\\ftpilegonder.bat 'ARTIST' 'SARKI'\n"
"\n"
"Kullanabilecegimiz kisaltmalar neler?\n"
"**Tikkat, butun kisaltmalar apostrof (') icinde yazilmalidir.\n"
"'SARKI' yazin Sarkinin adi gelsin.\n"
"'ARTIST' yazin sarkiyi soyleyenin adi gelsin.\n"
"'ALBUM' yazin albumun ismi gelsin.*\n"
"'DOSYA' yazin dosyanin tam adresi gelsin.\n"
"\n"
"* 'ALBUM' kismi sadece \"ID3 bilgisini girc girc\" aktif iken calisir.\n"
"\n"
"Nedir?\n"
"\n"
"Bu plug-in'in fikir babası Ed Zaleski (oddsock@oddsock.org)'dir. 2000 yılında, DoSomething olarak bu arkada$ tarafından yazılan kodlar baz alınarak hazırlanmı$tır. DoSomething bir i$e yaramadığı için tarafımdan, Erdinç AKAN (erakan@softhome.net) tekrar yazılmı$tır.\n"
"\n"
"Neden?\n"
"\n"
"Yeni web siteme \"$u anda $u $arkıyı dinliyorum\" gibi artistik hareketler yerlestirdim. Tabii bunu yapmak iki a$amalı bir i$. Birincisi web sitesini ayarlamak, ikincisi Winamp ile bir plug'in çalı$tırmak. Birinci kısım sorun değildi. 10-15 satırlık bir ASP programı ile halledildi. Winamp kısmı ise, biraz kastırtacaktı. Benimde hiç kasmaya niyetim olmadığı için aldım elime Google'i ba$ladım Winamp ve Blog kelimelerini aramaya. Derken kar$ıma Ed'in yazdığı plug-in çıktı. Sitesinde kaynak kodları da dağıtıyordu. Programlama ile uğra$maya hiç niyetim olmadığı için direkt binary'lerini indirdim kurdum. Tabii çalı$madı. Saçmaladı durdu. Dedim ba$ka plug-in'ler arayayım onları kullanayım. Bir iki tane daha buldum ama onlar da benim istediğimi yapmıyorlardı. Sonuçta i$ ba$a dü$tü. Ed'in kodlarını indirip, neyi nasıl yaptığına baktıktan sonra bo$ bir cumartesi ak$amı bu programı kodlamaya ba$ladım. Ertesi güne $u anda kullandığınız sürüm hazırdı.\n"
"\n"
"Niye ki?\n"
"\n"
"Çünkü plug-in'in aslı, URL gönderirken sapıtıyordu, zart-zurt çöküyordu, düzeltmek gerekiyordu. Ed'in yazdığında XML, HTML olu$turma, FTP'den dosya upload etme gibi bi yiğin ıvır zıfır vardı. Bunları yazmadım. Bana sadece URL submit yeterliydi. Fakat ba$ka arkada$ların deği$ik eyilimleri olabileceği aklıma geldiği için komut çalı$tırmayı yaptim. Lakin hem XML-HTML, hem de FTP upload i$ini BAT dosyalarla, oda olmadi yazacaginiz programlarla çok kolay yapabilenzi değil mi cevat ağbi? (evet)\n"
"\n"
"Nereden?\n"
"\n"
"Bu plug-in'i GNU General Public License altında open source olarak dagitiyorum. Hemen artizlik yapip adamin kaynak kodlari calmis biraz kurcalamis yapmis demeyin, lakin adamin kodlari felaket, isteyen olursa diye kendi sitemden diger kodlari da dagitiyorum. XML isteyen bu kodlari kurcalayabilir (Allah sabir vere ;) ). Bunun disinda kodlarin %90'i degisti. Web sitemden daha ayrintili bilgi alabilir. ASP ile beraber bu olay nasil yapiliyor ogrenebilirsiniz.\n"
"\n"
"web: http://erdincakan.rulez.it\n"
"mayil: erakan@softome.net\n"
"\n"
"Hak hukuk cart curt?\n"
"\n"
"Bu plug-in size olduğu gibi verilmektedir. Ekinde kaynak kodları da vardır. Bu plug-in'i sisteminize kurmamalısınız. Kullanmamalısınız. Ben bu plug-in'in ne olduğunu bilmiyorum. Ben kimimim, siz kimsiniz bilmiyoum. Sizin bilgisayarınızı bozan program bu değil. Ben görmedim, duymadım hakim bey. Ben ne bilem?\n"
"\n"
"Aaa $unu da eklesene?\n"
"\n"
"Sen eklesene?!!\n"
"\n"
"Gelecek?\n"
"\n"
"Programda ID3v1 okuma var, bunun yanına ID3v2 eklenebilir de, fakat o kadar bo$ zamanım yok. ID3v2, çok karma$ık, charset'den tutun, $ifrelemeye kadar bir yığın $ey eklenebiliyor ID3v2'ye. Bütün bunları kodlamak da bana zor geliyor...\n"
"\n"
"Kodlar?\n"
"\n"
"Paul DiLascia'nın dediği gibi: \"Kodlar çalı$ıyorsa, Erdinç AKAN erakan@softhome.net tarafından yazılmı$tır. Çalı$mıyorsa kimin yazdığını bilmiyorum!..\"\n"
"\n"
"Son sözleriniz?\n"
"\n"
"Kırmızı teldi, evet evet, bide onu keseyim bakalım...\n"
"\n"
"\n"
"\n"
"\n"
"Hepsini okudun mu? Aferim ;)\n"
"(eof)"
;

#endif // #ifndef EA_ACIKLAMATEXTI_INCLUDED