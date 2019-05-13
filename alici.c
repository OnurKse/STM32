#include <16f877.h>
#include <stdlib.h>
#include <stdint.h>
#fuses HS,NOPROTECT,NOBROWNOUT,NOLVP
#use delay (clock=20000000)
#use rs232 (baud=9600,xmit=pin_C6,rcv=pin_C7,stop=1,parity=N,bits=8)
#define  dht11 PIN_B4 
#include "DHT11.C"
#use fast_io(b)
#use fast_io(c)
int16 RR=0;
int Buffer=0;
int i=0;
int eskiout=0;
unsigned int sicaklik=-10;
unsigned int nem=-10;
int data=0;
int asas=0;
float RPS=0;
int16 kesme = 0;
void arttir(void);
void azalt(void);
void dur(void);
void yon(void);
void sicaklikGonder(void);
void humidityGonder(void);
void rpm(void);

int32 numPulses = 0;
int32 rpm_value = 0;
int32 cntTimerInt = 0;

#pragma     BYTE         RCSTA   = 0x18
//------------------------------------------------
void  RS232_Reset()                             
{
   if(bit_test(RCSTA, 1))
   {
      RCSTA=0;
      delay_us(1);
      RCSTA=0x90;
   }
}
#int_ext
void Rb0_Kesmesi()
{
   numPulses++;
}

#int_timer0
void timer0_kesmesi()
 {
   cntTimerInt++;
   if(cntTimerInt == 100)   // 10ms kesme oluþma süresi, 1snlik periyot için 1/0.01=100
   {
      rpm_value = numPulses;
      //putc(rpm_value);
      numPulses = 0;
      cntTimerInt = 0;
   }
  
   set_timer0(60); 
 }              

#int_rda
void serihaberlesme_kesmesi ()
{
 
  // disable_interrupts(int_ext);
   disable_interrupts(int_rda);
   Buffer=getc();
   switch(Buffer)
   {
      case 0b00000001: 
      arttir();
      break;
      case 0b00000010:
      azalt();
      break;
      case 0b00000100:
      dur();
      break;
      case 0b00001000:
      yon();
      break;
      case 0b10000000:
      sicaklikGonder();
      break;
      case 0b11000000:
      humidityGonder();
      break;
      case 0b11100000:
      rpm();
      break;
   }
   Buffer=0;
  //enable_interrupts(int_ext);
  
}
void main(void)
{
setup_psp(PSP_DISABLED);
setup_adc_ports(NO_ANALOGS);
setup_adc(ADC_OFF);
setup_ccp1(CCP_PWM);
setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256); //20Mhz'de 10ms de bir kesme verir
set_timer0(60); 
setup_timer_2(T2_DIV_BY_1,249,1);
setup_timer_1(T1_DISABLED);
/*setup_timer_1(T1_EXTERNAL | T1_DIV_BY_1);*/
set_tris_d(0x00);
/*set_tris_c(0x01);*/
output_b(0x00);
output_d(0x00);
//dht_init();
/*set_timer1(65535); */
// Set RB0 to input, RB1-7 output 
set_tris_b(0x01);
output_b(0x00);
  ext_int_edge(L_TO_H);   // init interrupt triggering for button press 
  enable_interrupts(INT_EXT); 
  enable_interrupts(INT_RTCC);
  enable_interrupts(GLOBAL);

while(1)
{
      /*delay_ms(20);
 
      enable_interrupts(int_rda);
      //read_dht(); // DHT 11'in okumasý için gereken fonksiyon.
      delay_ms(1000);
      //sicaklik =(int)dht_dat[2]; // dht_dat[2] deðiþkenini bize sýcaklýk bilgisini verir bu bilgiyi sicaklik olarak tanýmladýðýmýz deðiþkene attým.
      //nem =(int)dht_dat[0];      // dht_dat[0] deðiþkenini bize nem bilgisini verir bu bilgiyi nem olarak tanýmladýðýmýz deðiþkene attým.*/
      rpm(); // test için eklendi
      RS232_Reset();
      if(kbhit())
         serihaberlesme_kesmesi();
      
}
}
void arttir(void)
{
   if(i>239)
   {
      i=249;
   }
   else 
    {
      i+=10;
    }
   set_pwm1_duty(i);
}
void azalt(void)
{
   if(i>10)
   {
      i-=10;
   }
   else
   {
      i=0;
   }
   set_pwm1_duty(i);
}
void dur(void)
{
output_low(pin_D0);
output_high(pin_D1);
set_pwm1_duty(0);
i=5;
}
void yon(void)
{
   eskiout=i;
   //durana kadar yavaþla
   for(;i>0;i--)
   {
      delay_ms(10);
      set_pwm1_duty(i);
   }
   //yön çevir
   output_toggle(pin_D0);
   output_toggle(pin_D1);
   //eski deðerine tekrar hýzlan
   for(;i<eskiout;i++)
   {
      delay_ms(10);
      set_pwm1_duty(i);
   }
}
void sicaklikGonder(void)
{
data=0b00100111;
putc(data);
putc(sicaklik);
}
void humidityGonder(void)
{
data=0b00101000;
putc(data);
putc(nem);
}
void rpm(void)
{
   data=0b11110000;
   putc(data);
   putc(rpm_value);
   delay_us(1000000);
   RR=0;
   kesme=0;
} 



