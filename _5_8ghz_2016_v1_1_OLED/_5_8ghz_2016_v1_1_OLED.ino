/*
  29.12.2016
  SerPanRC@yandex.ru
 http://forum.rcdesign.ru/f90/thread481340.html
  */
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <OzOLED.h>


#define RssiR_pin A7 // вход RSSI правый
#define RssiL_pin A6 // вход RSSI левый
#define U_pow_pin A0 //  вход напряжения питания
#define Rec 7 // пин  выбора ресивера
#define Bolche A3
#define Regim A2
#define Menche 9
#define Vvod A1

#define SSP 10

unsigned char lcd_x = 0, lcd_y = 0;
byte column=0; // cтолбец
byte line=0; // строка
byte contrast=15; // контрастность изображения дисплея
unsigned int n=0;
byte data0=0; 
byte data1=0;
byte data2=0;
byte data3=0;
unsigned int Freguency=0;
unsigned int Delitel;
byte DelitelH ;
byte DelitelL ;
byte rgm =1; // режим действующий, 0- диверсити 1- прием, 2- спектр, 3- скан   
byte rgm_zdn =0; // режим задание,
int sval_R=0;
int sval_L=0;
int sval_U=0;
int RSSI = 0;// максимум АРУ
int spec =0;
int MAXi = 0;// i при макс АРУ

boolean R_up =true; //     флаг что было больше на правом
unsigned long time;     // абсолютное время в момент изменения у какого приемника сигнал больше
const byte time_sw=60; //  минимальное время в мс для переключения приемников
const byte delta_RSSI=3; // минимальная дельта в RSSI для переключения приемников
byte Fr_otobr =0;  // флаг о том что еще не было отображения частоты на экране
byte R_res=1; // флаг - работает правый приемник
byte sw=1;

void setup()  
{ 
  
  pinMode(Rec, OUTPUT); 
  digitalWrite(Rec, 0);

  pinMode(Bolche, INPUT);
  digitalWrite(Bolche, HIGH);
  pinMode(Menche, INPUT);
  digitalWrite(Menche, HIGH);
  pinMode(Vvod, INPUT);
  digitalWrite(Vvod, HIGH);
  pinMode(Regim, INPUT);
  digitalWrite(Regim, HIGH);
 
 
 DelitelL= EEPROM.read(0);
 DelitelH= EEPROM.read(1);
 Freguency= (DelitelH *32 + DelitelL)*2 +479;
 if (Freguency>6000 || Freguency<5500) Freguency=5645;
 
 //CS - to digital pin 10  (SS pin)
 // * SDI - to digital pin 11 (MOSI pin)
 //* CLK - to digital pin 13 (SCK pin)
 //pinMode (slaveSelectPin, OUTPUT);
  pinMode (SSP, OUTPUT);
  
  OzOled.init();  //initialze OLED display
  OzOled.clearDisplay();           //clear the screen and set start position to top left corner
  OzOled.setNormalDisplay();       //Set display to Normal mode
  OzOled.setPageMode();            //Set addressing mode to Page Mode
  
  //LcdClear();
  
  // initialize SPI:
  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  
  digitalWrite(SSP,LOW);
  SPI.transfer(0x10);
  SPI.transfer(0x01);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(SSP,HIGH);
  
  prog_freg(); 
 
}
  
void loop()
{  
   if (rgm!=rgm_zdn)
   {
    rgm=rgm_zdn; 
    if (rgm==1)  OzOled.printString("RCV",12,0);
    if (rgm==2)  OzOled.printString("SPC",12,0);
    if (rgm==3)  OzOled.printString("SCN",12,0);
    if (rgm==0)  
        {
          OzOled.printString("DIV",12,0);
          Fr_otobr=0;
          OzOled.printString("RSSI-R:",0, 1);
          OzOled.printString("RSSI-L:         ",0, 2);
          OzOled.printString("Upow:",0, 6);
         }
   }
  
    while (digitalRead(Regim)==LOW)
     {
       rgm=rgm+1;
      if (rgm>3) rgm=0 ;
      if (rgm==1)  OzOled.printString("RCV",12,0);
      if (rgm==2)  OzOled.printString("SPC",12,0);
      if (rgm==3)  OzOled.printString("SCN",12,0);
      if (rgm==0)  
        {
          OzOled.printString("DIV",12,0);
          Fr_otobr=0;
          OzOled.printString("RSSI-R:",0, 1);
          OzOled.printString("RSSI-L:         ",0, 2);
          OzOled.printString("Upow:",0, 6);
         }
      delay (800);
      rgm_zdn=rgm;
     }

 
    
    switch (rgm) 
 {  
   
   /*    ------------------ДИВЕРСИТИ------------------------*/
   
       case 0:
       
    if (Fr_otobr==0)  // если еще не вывели на экран значение частоты то выводим
     {
       Fr_otobr=1;
       OzOled.printString("FREG:      ",0, 0);
       OzOled.printNumber((long)Freguency,5,0);
     }
     
    sval_R=analogRead(RssiR_pin); 
    sval_L=analogRead(RssiL_pin);
    if ((sval_R-sval_L)>delta_RSSI)
     {
      if (R_up !=true) // если правый стал больше
       { 
         R_up =true;
         time = millis(); // новое время отчета
       }
      else // если правый опять больше
        {
          if (millis()-time>=time_sw and R_res==0) {digitalWrite(Rec, 0); R_res=1; }// то при при прохождении времени включаем правый
        }
     }
    if ((sval_L-sval_R)>delta_RSSI)
     {
      if (R_up ==true) // если левый стал больше
       { 
         R_up =!true;
         time = millis(); // новое время отчета
       }
      else
        {
          if (millis()-time>=time_sw and R_res==1) {digitalWrite(Rec, 1);  R_res=0;}
        }
     }
   
     Bolche_Menche();
      if (sw<100)
      {
          sw=sw+1;
          if (sval_R>140) 
            {
              OzOled.printNumber((long)((10*sval_R-1400)/4),7,1);
            }
          else 
            {
              OzOled.printString("   0",7,1);
            }
      
          if (sval_L>140) 
            {
               OzOled.printNumber((long)((10*sval_L-1400)/4),7,2);
            }
          else 
            {
               OzOled.printString("   0",7,2);
            }
      }
      else
      {
        sw=0;
         sval_U=0; 
    for ( byte i = 0; i < 20; i++)
     {
      sval_U = sval_U + analogRead(U_pow_pin);
       delay (5);
     }
        OzOled.printNumber((float) sval_U/1000   ,2,5,6);
      }
   
       Vvod_();
       break;
      
      /*-------------------------------РЕСИВЕР--------------------------------------------*/
    case 1:
    sval_R=0; 
    for ( byte i = 0; i < 10; i++)
     {
      sval_R = sval_R + analogRead(RssiR_pin);
       delay (5);
     }
 
  Bolche_Menche(); 
  
  OzOled.printString("RSSI-R:     ",0,1);
  if (sval_R>1400) {
    OzOled.printNumber((long)((sval_R-1400)/4),7,1);
    byte L=(sval_R-1380)/20;
    if (L>127) L=127;
    H_Bar (L,4);
    
   
       }
   else 
     {
       OzOled.printString("   0",7,1);
       H_Bar (0,4);
      }
     sval_U=0; 
    for ( byte i = 0; i < 20; i++)
     {
      sval_U = sval_U + analogRead(U_pow_pin);
       delay (5);
     }
        OzOled.printNumber((float) sval_U/1000   ,2,5,6);
      
   Vvod_();
   break;
      
      /* -----------------------------СПЕКТР---------------------------*/
    case 2:
   
    for ( byte i = 0; i < 125; i++)
    {
      Freguency = 5500+4*i;
      prog_freg();
      
      OzOled.printString("FREG: ",0,0);
      OzOled.printNumber((long)Freguency,5,0);
      if (digitalRead(Bolche)==LOW) delay (500);  // при нажатии на кнопку замедляем сканирование
      delay (20);
      sval_R=0;
      for ( byte u = 0; u < 10; u++)
       {
          sval_R = sval_R + analogRead(RssiR_pin);
          delay (1);
        }
      if (sval_R<1280) sval_R=1280 ;
      spec =(sval_R-1280)/40;
       column=i;
       Lcd_drawColumn(1,spec);
       column=i+1;
       Lcd_drawColumn(1, 0);
       if (digitalRead(Regim)==LOW )
        { 
          rgm_zdn=0;
          DelitelL= EEPROM.read(0);
          DelitelH= EEPROM.read(1);
          Freguency= (DelitelH *32 + DelitelL)*2 +479;
          if (Freguency>6000 || Freguency<5500) Freguency=5645;
          prog_freg();
          OzOled.printNumber((long)Freguency,5,0);
 
          for ( byte m = 0; m < 125; m++)   // очищаем от спектра
            {
             column=m;
             Lcd_drawColumn(1, 0);
            }
          goto prer ; 
        }
     }  
      prer:
      break;
      
      
      case 3:
      sval_R=0;
      RSSI=0;
     
      for ( byte i = 0; i < 249; i++)
   {
      Freguency = 5501+2*i;
      prog_freg();
      OzOled.printString("FREG: ",0,0);
      OzOled.printNumber((long)Freguency,5,0);
  
      delay (50) ;
      OzOled.printString("RSSI:       ",0,1);
      sval_R =analogRead(RssiR_pin);
      OzOled.printNumber((long)sval_R,5,1);
      if (sval_R > RSSI )
        { 
          RSSI=sval_R;
          MAXi=i ; 
        }
     }
   Freguency = 5501+2*MAXi;
   prog_freg();
   rgm_zdn=0;
   break;    
  
  }
 }

