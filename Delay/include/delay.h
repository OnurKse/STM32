#ifndef _DELAY_H
#define _DELAY_H

#include "cpu.h"

#define delay_const  (unsigned long) (336000000 / 15574784)

/* Private function prototypes -----------------------------------------------*/
void delay_ms(__IO uint32_t nTime);
void delay_us(__IO uint32_t xx);
void TimingDelay_Decrement(void);

#endif
