#include <16f877.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#fuses HS,NOPROTECT,NOBROWNOUT,NOLVP
#use delay (clock=20000000)
#use rs232 (baud=9600,xmit=pin_C6,rcv=pin_C7,stop=1,parity=N,bits=8)
//#define  dht11 PIN_B4 
//#include <DHT11.C>
#use fast_io(b)
#use fast_io(c)

#define HASSASIYET 50            //  (  50)Kac milisaniyede bir rmp hesabý yapýlacak

int32 numPulses = 0;
int32 rpm = 0;
int32 cntTimerInt = 0;

#int_EXT
void ext_kesmesi() 
{
  numPulses++;; 
}

#int_timer0
void timer0_kesmesi()
 {
      cntTimerInt++;
   if(cntTimerInt == HASSASIYET / 2)            // (  2  )[HASSASIYET / 10ms] tane kesme gelince rpm'yi guncelle
   {
   
   
      rpm = numPulses * (50 / HASSASIYET) *60;   // (50 / HASSASIYET)*60 ; 
      numPulses = 0;
      cntTimerInt = 0;
   }
  
   
   set_timer0(100);   // "RTCC_DIV_256 ve kristal 20MHz" oldugunda 13.1 mili saniyede bir kesme veriyor.  (100)
 }                     // Eger set_rtcc(60) denirse kesmeler

void main()                      
{
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256); //20Mhz'de 13.1ms de kesme verir
   ext_int_edge(L_to_H);
   enable_interrupts(INT_EXT);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);


   while(1)
   {
     putc(rpm);
     delay_ms(500);
   }
}
