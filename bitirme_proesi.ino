#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//#include <deprecated.h>
//#include <MFRC522.h>
#include <SPI.h>
#include <virtuabotixRTC.h>



#define pir A1
#define pir1 A2
#define hall A0
#define buz 3
#define but 2

byte ID[3][4]={{62,207,165,185},{239,212,238,41}};
char sifreler[5]={'1','2','3','4','C'};
char girilensifre[5]={};
byte ensongiren[4]={'0','0','0','0'};
byte ensoncikan[4]={'0','0','0','0'};

unsigned int g_girensayisi=0,h_girensayisi=0,a_girensayisi=0,y_girensayisi=0;
unsigned int g_cikinsayisi=0,h_cikinsayisi=0,a_cikinsayisi=0,y_cikinsayisi=0;
int szaman=0,ozaman=0,sayici=0,kalanzaman=15,szaman1,ozaman1,szaman2,ozaman2,day;
int saatg,dakg,sang,saatc,dakc,sanc;
int RST_PN=49;
int SS_PN=53;
byte girishakki=1,saatd=1;
boolean dogru=0,izin=0,ileri=0,giren,cikan;
int artir,azalt,menusayac=-1,ksayac=0;
int durumk=4,i=0,sayac=6,j=0;

const byte satir=4;
const byte sutun=4;

char tus[4][4]={
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'#','0','*','D'}
};

byte pirs[2]={};
byte halls[1]={};

byte satir_pin[satir]={11,10,9,8};
byte sutun_pin[sutun]={7,6,5,4};

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PN,RST_PN);
Keypad tus_takimi = Keypad(makeKeymap(tus),satir_pin,sutun_pin,satir,sutun);
virtuabotixRTC RTC(14,15,16);

void setup() 
{
  pinMode(pir,INPUT);
  pinMode(pir1,INPUT);
  pinMode(hall,INPUT_PULLUP);
  pinMode(but,INPUT);
  pinMode(buz,OUTPUT);
  lcd.begin();
  lcd.backlight();
  SPI.begin();
  rfid.PCD_Init();
  Serial.begin(19200);
  
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Sisteme");
  lcd.setCursor(2,1);
  lcd.print("Hoslgeldiniz");
  delay(1500);
  lcd.clear();
  RTC.setDS1302Time(00, 38, 12,6, 4, 9, 2021);

}


void loop() 
{
  szaman=millis();
  szaman1=millis();
  durum();
  saat(saatd);
  menu();
  if(RTC.hours==00 && RTC.minutes==00 && RTC.seconds==00)
  {
     day++;
     g_girensayisi=0;
     g_cikinsayisi=0;
  }
  if(day==7)
  {
     h_girensayisi=0;
     h_cikinsayisi=0;
  }
  if(day==30)
  {
     a_girensayisi=0;
     a_cikinsayisi=0;
  }
  if(day==365)
  {
     y_girensayisi=0;
     y_cikinsayisi=0;
     day=0;
  }
  
  
  

}
void durum()
{
  
  
  // sensör okuma başlangıç
  if(digitalRead(pir)==1) pirs[0]=1;
  else pirs[0]=0;
  if(digitalRead(pir1)==1) pirs[1]=1;
  else pirs[1]=0;
  delay(1000);
  if(digitalRead(hall)==1) halls[0]=1;
  else halls[0]=0;
  // sensör okuma bitimi
for(int i=0;i<2;i++)
{
  Serial.print("pir:");
  Serial.println(pirs[i]);
  
}
for(int i=0;i<1;i++)
{
  Serial.print("hall:");
  Serial.println(halls[i]);
}
  Serial.println("--------------------");
  if(szaman-ozaman>250)
  {
   ozaman=szaman;
  if(pirs[0]==1 && pirs[1]==1 && halls[0]==1) durumk=0;
  if(pirs[0]==1 && pirs[1]==0 && halls[0]==1) durumk=1;
  if(pirs[0]==0 && pirs[1]==1 && halls[0]==1) durumk=2;
  if(pirs[0]==0 && pirs[1]==0 && halls[0]==1) durumk=3;
  if(pirs[0]==0 && pirs[1]==0 && halls[0]==0) durumk=4;
    
    
  }
  Serial.println(durumk);
  switch(durumk)
  {
    case 0:
    Serial.println("HATA");
    error();
    durumk=5;
    break;
    case 1:
    Serial.println("CIKIS");
    izin=1;
    lcd.clear();
    durumk=5;
    cik(izin);
    break;
    case 2:
    Serial.println("GIRIS");
    izin=1;
    durumk=5;
    gir(izin);
    break;
    case 3:
    Serial.println("KAPI ACIK");
    ksayac++;
    Serial.print("ksayac:");
    Serial.println(ksayac);
    if(ksayac==45)
    {
      alarm();
      ksayac=0;
    }
    break;
    case 4:
    ksayac=0;
    Serial.println("BIR SEY YOK");
    break;
  }
  izin=0;
}
void kalansure()
{
  szaman2=millis();
  if(szaman2-ozaman2>250)
    {
      ozaman2=szaman2;
      sayici++;
      if(sayici==4)
      {
        sayici=0;
        kalanzaman--;
        Serial.println(kalanzaman);
        lcd.setCursor(12,0);
        lcd.print(kalanzaman);
        if(kalanzaman==0)
        {
        kalanzaman=15;
        girishakki--;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("giris yapilmedi");
        lcd.setCursor(0,1);
        lcd.print("hak:");
        lcd.setCursor(4,1);
        lcd.print(girishakki);
        delay(2500);
        lcd.clear();
        if(girishakki==0) alarm();
        }
      }
    }
}



void gir(boolean izin)
{
  lcd.clear();
  while(izin==1)
  {
    if(girishakki>0)
    {
      giren=1;
      kartoku();
      if(ileri == 1 && girishakki>0)
      {
       sifre();
       izin=0;
      }
    }
   
   Serial.println("burada");
  }
  g_girensayisi++;
  h_girensayisi++;
  a_girensayisi++;
  y_girensayisi++;
}

void cik(boolean izin)
{
  while(izin==1)
  { 
    if(girishakki>0)
    {
      cikan=1;
      Serial.println("*1");
      kartoku();
    }
    if(ileri==1)
      {
        Serial.println("*2");
        izin=0;
        ileri=0;
        break;
      }
    Serial.println("Çıkışta:");
    Serial.println(izin);
  }
  g_cikinsayisi++;
  h_cikinsayisi++;
  a_cikinsayisi++;
  y_cikinsayisi++;
} 
void error()
{
  while(! dogru)
  {
    char cik=tus_takimi.getKey();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HATA");
    digitalWrite(buz,1);
    delay(100);
    szaman=millis();
    if(szaman-ozaman>400)
    {
      ozaman=szaman;
      digitalWrite(buz,0);
      delay(150);
    }
    if(cik=='D')
    {
      if(digitalRead(but)==1)
      {
      digitalWrite(buz,0);
      girishakki=3;
      break;
      }
    }
  }
}
void alarm()
{
  while(! dogru)
  {
    char cikis=tus_takimi.getKey();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ALARM");
    digitalWrite(buz,1);
    delay(100);
    szaman=millis();
    if(szaman-ozaman>400)
    {
      ozaman=szaman;
      digitalWrite(buz,0);
      delay(150);
    }
    if(cikis=='D')
    {
      if(digitalRead(but)==1)
      {
      digitalWrite(buz,0);
      girishakki=1;
      ksayac=0;
      break;
      }
    }
  }
}
void saat(int saat)
{
  int s=saat;
  if(s==1)
  {
    if(szaman1-ozaman1>1000)
    {
    lcd.clear();
    ozaman1=szaman1;
    RTC.updateTime();
    lcd.setCursor(0,0);
    lcd.print("Tarih:");
    lcd.setCursor(6,0);
    lcd.print(RTC.dayofmonth);
    lcd.setCursor(9,0);
    lcd.print(RTC.month);
    lcd.setCursor(11,0);
    lcd.print(RTC.year);
    lcd.setCursor(0,1);
    lcd.print("Saat:");
    lcd.setCursor(5,1);
    lcd.print(RTC.hours);
    lcd.setCursor(8,1);
    lcd.print(RTC.minutes);
    lcd.setCursor(11,1);
    lcd.print(RTC.seconds);
    }
  } 
}
void kartoku()
{
  while(girishakki>0 && izin==1 && ileri==0)
  {
  lcd.setCursor(0,0);
  lcd.print("kalan zaman:");
  lcd.setCursor(14,0);
  lcd.print("sn");
  kalansure();
  if(! rfid.PICC_IsNewCardPresent()) return;
  if(! rfid.PICC_ReadCardSerial()) return;
  if((rfid.uid.uidByte[0]==ID[0][0] &&
  rfid.uid.uidByte[1]==ID[0][1] &&
  rfid.uid.uidByte[2]==ID[0][2] &&
  rfid.uid.uidByte[3]==ID[0][3]) ||
  (rfid.uid.uidByte[0]==ID[1][0] &&
  rfid.uid.uidByte[1]==ID[1][1] &&
  rfid.uid.uidByte[2]==ID[1][2] &&
  rfid.uid.uidByte[3]==ID[1][3]) )
  {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Yetkili kart");
  ileri=1;
  sayici=0;
  kalanzaman=30;
  delay(1000);
  lcd.clear();
  /*for(int i=0;i<rfid.uid.size;i++)
    {
    lcd.setCursor(i*3,1);
    lcd.print(rfid.uid.uidByte[i],HEX);
    }*/
  }
  
  else
  {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Yetkisiz Kart");
  delay(1000);
  girishakki--;
  }
  if(giren==1)
  {
    for(int i=0;i<rfid.uid.size;i++)
    {
    ensongiren[i]=rfid.uid.uidByte[i];
    }
    saatg=RTC.hours;
    dakg=RTC.minutes;
    sang=RTC.seconds;
    giren=0;
  }
  if(cikan==1)
  {
    for(int i=0;i<rfid.uid.size;i++)
    {
    ensoncikan[i]=rfid.uid.uidByte[i];
    }
    saatc=RTC.hours;
    dakc=RTC.minutes;
    sanc=RTC.seconds;
    cikan=0;
  }
  rfid.PICC_HaltA();
 }
}
void sifre()
{
  while(izin==1)
  {
  char tus_bas=tus_takimi.getKey();
  szaman=millis();
  lcd.setCursor(0,0);
  lcd.print("kalan zaman:");
  lcd.setCursor(14,0);
  lcd.print("sn");
  lcd.setCursor(0,1);
  lcd.print("Sifre:");
  kalansure();
 if(tus_bas) 
  {
    if(szaman-ozaman>100)
    {
      ozaman=szaman;
      if(tus_bas!='D')
      {
      lcd.setCursor(sayac,1);
      lcd.print(tus_bas);
      delay(100);
      lcd.setCursor(sayac,1);
      lcd.print("*");
      girilensifre[i]=tus_bas;
      i++;
      sayac++;
        if(sayac==11 && girilensifre[4]=='C')
        {
          for(int j=0;j<4;j++)
          {
            if(sifreler[j]==girilensifre[j]) dogru=true;
            else if(sifreler[j]!=girilensifre[j])
            {
            girishakki--;
            dogru=false;
            sayac=6;
            i=0;
            if(girishakki>0) break;
            if(girishakki==0)
            {
              for(int a=0; a<5 ; a++)
              {
                girilensifre[a]='0';
              }
              alarm();
              break;
            }
            
            }
          }
          
          if(dogru==true)
          {
           kalanzaman=30;
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("girilen sifre");
           lcd.setCursor(0,1);
           lcd.print("dogru");
           girishakki=3;
           sayac=6;
           i=0;
           ileri=0;
           izin=0;
           delay(1000);
           lcd.clear();
           dogru=false;
          }
          else
          {
           kalanzaman=30;
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("girilen sifre");
           lcd.setCursor(0,1);
           lcd.print("yanlis");
           lcd.setCursor(8,1);
           lcd.print("hak:");
           lcd.setCursor(13,1);
           lcd.print(girishakki);
           delay(1000);
           lcd.clear();
           sayac=6;
           i=0;
          }
      }
    }
      if(tus_bas=='D')
      {
       lcd.clear();
       lcd.print("Sifre:");
       sayac=6;
       i=0;
        for(int a=0; a<4 ; a++)
        {
         girilensifre[a]='0';
        } 
      }
  }
 }          
}
}
void menu()
{
  
  while(digitalRead(hall)!=1)
  {
  char tus_tak=tus_takimi.getKey();
  if(tus_tak)
  {
    saatd=0;
    if(tus_tak=='A')
    {
      menusayac++;
      lcd.clear();
      if(menusayac==8) menusayac=7;
    }
    if(tus_tak=='B')
    {
      menusayac--;
      lcd.clear();
      if(menusayac==-1) menusayac=0;
    }
    switch(menusayac)
    {
      case 0:
      lcd.setCursor(0,0);
      lcd.print("G: giris/cikis");
      lcd.setCursor(0,1);
      lcd.print(g_girensayisi);
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(g_cikinsayisi);
      break;
      case 1:
      lcd.setCursor(0,0);
      lcd.print("H: giris/cikis");
      lcd.setCursor(0,1);
      lcd.print(h_girensayisi);
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(h_cikinsayisi);
      break;
      case 2:
      lcd.setCursor(0,0);
      lcd.print("A: giris/cikis");
      lcd.setCursor(0,1);
      lcd.print(a_girensayisi);
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(a_cikinsayisi);
      break;
      case 3:
      lcd.setCursor(0,0);
      lcd.print("Y: giris/cikis");
      lcd.setCursor(0,1);
      lcd.print(y_girensayisi);
      lcd.setCursor(8,1);
      lcd.print("/");
      lcd.setCursor(9,1);
      lcd.print(y_cikinsayisi);
      break;
      case 4:
      lcd.setCursor(0,0);
      lcd.print("Enson giris yapan");
      lcd.setCursor(0,1);
      for(int i=0;i<rfid.uid.size;i++)
      {
        lcd.setCursor(i*3,1);
        lcd.print(ensongiren[i],HEX);
      }
      
      break;
      case 5:
      lcd.setCursor(0,0);
      lcd.print("Enson Cikis ypn");
      lcd.setCursor(0,1);
      for(int i=0;i<rfid.uid.size;i++)
      {
        lcd.setCursor(i*3,1);
        lcd.print(ensoncikan[i],HEX);
      }
      break;
      case 6:
      lcd.setCursor(0,0);
      lcd.print("Enson giris zmn");
      lcd.setCursor(0,1);
      lcd.print(saatg);
      lcd.setCursor(3,1);
      lcd.print(dakg);
      lcd.setCursor(6,1);
      lcd.print(sang);
      break;
      case 7:
      lcd.setCursor(0,0);
      lcd.print("Enson Cikis zmn");
      lcd.setCursor(0,1);
      lcd.print(saatc);
      lcd.setCursor(3,1);
      lcd.print(dakc);
      lcd.setCursor(6,1);
      lcd.print(sanc);
      break;
    }
    if(tus_tak=='D' || digitalRead(hall)!=0 ) 
    {
      menusayac=-1;
      saatd=1;
    }
  }
}
saatd=1;
}
