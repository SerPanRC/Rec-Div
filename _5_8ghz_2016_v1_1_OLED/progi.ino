
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
       OzOled.printString("FREG:",0,0);
       OzOled.printNumber((long)Freguency,5,0);
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
   
 void H_Bar (byte P,byte y)
  {
   OzOled.setCursorXY(0,y);
   for (int i=0; i <= 127; i++)
    {
      if (P>=i) {OzOled.sendData(255);}
      else OzOled.sendData(0);
    }
  }
  
  void Lcd_drawColumn(unsigned char sline, unsigned char value){//строк пустых наверху ,уровень что отражаем

    unsigned char lines=9-sline;
    if (value > lines*8) value = lines*8;
    unsigned char mark = (lines*8 - 1 - value)/8;
    for (unsigned char line = 0; line < mark; line++) { 
        OzOled.setCursorXY_L(column, sline + line);
        OzOled.sendData(0x00);
        }
        
    unsigned char b = 0xff;
    for (unsigned char i = 0; i < lines*8 - mark*8 - value; i++) {
        b <<= 1;
    }
    OzOled.setCursorXY_L(column, sline + mark);
    OzOled.sendData( b);
    // Fill the lines below the mark...
    for (unsigned char line = mark + 1; line < lines; line++) {
        OzOled.setCursorXY_L(column, sline + line);
        OzOled.sendData( 0xff);
    } 
    //lcd_set_pos_pix(column +1, sline);
  }
  
