
 void prog_freg (void) {
  
 Delitel=(Freguency -479)/2 ;
      DelitelH=Delitel>>5 ;
      DelitelL=Delitel &0x1F;
      data0=DelitelL *32 +17 ;
      data1=DelitelH *16 + DelitelL /8;
      data2=DelitelH /16 ;
 
      digitalWrite(SSP,LOW);
      SPI.transfer(data0);
      SPI.transfer(data1);
      SPI.transfer(data2);
      SPI.transfer(data3);
      digitalWrite(SSP,HIGH);}
      
      
 void Bolche_Menche (void)
  {
        if (digitalRead(Bolche)==LOW or digitalRead(Menche)==LOW)
     {
       n=n+1;
       if (digitalRead(Bolche)==LOW and n<15) Freguency=Freguency+2;
       if (digitalRead(Bolche)==LOW and n>=15) Freguency=Freguency+10;
       if (digitalRead(Menche)==LOW and n<15 )  Freguency=Freguency-2;
       if (digitalRead(Menche)==LOW and n>=15 )  Freguency=Freguency-10;
       
       if (Freguency <5500) Freguency=5999;
       if (Freguency >6000) Freguency=5501;  
       prog_freg();
       lcd_set_pos_pix(0, 0);
       LcdString("FREG: ");
       LcdString_f(Freguency);
       delay(200);
      } 
     else { n=0;}
  }
  
   void Vvod_ (void)
   {
   if (digitalRead(Vvod)==LOW)  
   {
     EEPROM.write(0, DelitelL);
     EEPROM.write(1, DelitelH);
    delay(3000);
    }
   }
