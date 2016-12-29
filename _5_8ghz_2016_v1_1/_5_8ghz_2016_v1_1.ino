/*
  29.12.2016
  SerPanRC@yandex.ru
 http://forum.rcdesign.ru/f90/thread481340.html
  */
#include <SPI.h>
#include <EEPROM.h>

#define PIN_SCE 5 //  XCS 
#define PIN_SDIN 4 // SDA
#define PIN_RESET 6 // XRES
#define PIN_SCLK 3 // SCLK

#define RssiR_pin A7 // вход RSSI правый
#define RssiL_pin A6 // вход RSSI левый
#define U_pow_pin A0 //  вход напряжения питания
#define Rec 7 // пин  выбора ресивера
#define Bolche A3
#define Regim A2
#define Menche 9
#define Vvod A1

#define SSP 10

#define LCD_C LOW // Command
#define LCD_D HIGH // Data

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
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  pinMode(Rec, OUTPUT); 

  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_SCE, HIGH);
  digitalWrite(PIN_SCLK, LOW);
  digitalWrite(PIN_SDIN, LOW);
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
   
  LcdInitialise();
  lcd_contrast(contrast);
   //LcdWrite(LCD_C, 0xA7); // Inverted display
  LcdWrite(LCD_C, 0xA6); // Normal display
  LcdClear();
  
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
    lcd_set_pos_pix(65, 0);
    if (rgm==1)  LcdString("RCV");
    if (rgm==2)  LcdString("SPC");
    if (rgm==3)  LcdString("SCN");
    if (rgm==0)  
        {
          LcdString("DIV");
          Fr_otobr=0;
          lcd_set_pos_pix(0, 1);
          LcdString("RSSI-R:");
          lcd_set_pos_pix(0, 2);
          LcdString("RSSI-L:         ");
          lcd_set_pos_pix(0, 5);
          LcdString("Upow:");
         }
   }
  
    while (digitalRead(Regim)==LOW)
     {
       rgm=rgm+1;
      if (rgm>3) rgm=0 ;
      lcd_set_pos_pix(65, 0);
      if (rgm==1)  LcdString("RCV");
      if (rgm==2)  LcdString("SPC");
      if (rgm==3)  LcdString("SCN");
      if (rgm==0)  
        {
          LcdString("DIV");
          Fr_otobr=0;
          lcd_set_pos_pix(0, 1);
          LcdString("RSSI-R:");
          lcd_set_pos_pix(0, 2);
          LcdString("RSSI-L:         ");
          lcd_set_pos_pix(0, 5);
          LcdString("Upow:");
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
       lcd_set_pos_pix(0, 0);
       LcdString("FREG: ");
       LcdString_f(Freguency);
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
          lcd_set_pos_pix(40, 1);
          if (sval_R>140) 
            {
              LcdString_f(((10*sval_R-1400)/4));
            }
          else 
            {
              LcdString("   0");
            }
      
          lcd_set_pos_pix(40, 2);
          if (sval_L>140) 
            {
               LcdString_f(((10*sval_L-1400)/4));
            }
          else 
            {
               LcdString("   0");
            }
      }
      else
      {
        sw=0;
        lcd_set_pos_pix(30, 5);
         sval_U=0; 
    for ( byte i = 0; i < 19; i++)
     {
      sval_U = sval_U + analogRead(U_pow_pin);
       delay (5);
     }
        LcdString_f(sval_U/10);
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
  
  lcd_set_pos_pix(0, 1);
  LcdString("RSSI-R:     ");
  lcd_set_pos_pix(40, 1);
  if (sval_R>1400) {
    LcdString_f(((sval_R-1400)/4));
    byte L=(sval_R-1380)/20;
    if (L>95) L=95;
    for (byte x = 0; x < L; x ++) 
      { 
        lcd_set_pos_pix(x , 2);
        LcdWrite(LCD_D, 0xff);}
        for (byte x = L; x < 96; x ++)
         { 
           lcd_set_pos_pix(x , 2);
           LcdWrite(LCD_D, 0x00);
         }
       }
   else 
     {
       LcdString("   0");
       for (byte x = 0; x < 96; x ++)
         { 
            lcd_set_pos_pix(x , 2);
            LcdWrite(LCD_D, 0x00);
          }
      }
    lcd_set_pos_pix(30, 5);
     sval_U=0; 
    for ( byte i = 0; i < 19; i++)
     {
      sval_U = sval_U + analogRead(U_pow_pin);
       delay (5);
     }
        LcdString_f(sval_U/10);
      
   Vvod_();
   break;
      
      /* -----------------------------СПЕКТР---------------------------*/
    case 2:
   
    for ( byte i = 0; i < 95; i++)
    {
      Freguency = 5525+5*i;
      prog_freg();
      
      lcd_set_pos_pix(0, 0);
      LcdString("FREG: ");
      LcdString_f(Freguency);
      if (digitalRead(Bolche)==LOW) delay (500);  // при нажатии на кнопку замедляем сканирование
      delay (20);
      sval_R=0;
      for ( byte u = 0; u < 10; u++)
       {
          sval_R = sval_R + analogRead(RssiR_pin);
          delay (1);
        }
      if (sval_R<1280) sval_R=1280 ;
      spec = 4+(sval_R-1280)/40;
      //spec =(sval_R-1200)/40;
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
          prog_freg();
 
          for ( byte m = 0; m < 95; m++)   // очищаем от спектра
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
      lcd_set_pos_pix(0, 0);
      LcdString("FREG: ");
      LcdString_f(Freguency);
  
      lcd_set_pos_pix(0, 1);
      delay (50) ;
      LcdString("RSSI:       ");
      sval_R =analogRead(RssiR_pin);
      lcd_set_pos_pix(35, 1);
      LcdString_f(sval_R);
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

