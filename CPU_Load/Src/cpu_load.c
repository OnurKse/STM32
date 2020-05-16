/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "cpu_load.h"

static uint8_t GENERAL_DWTCounterEnable(void);

uint8_t CPULOAD_Init(CPULOAD_t* CPU_Load) {
	/* Set values to 0 */
	CPU_Load->Load = 0;
	CPU_Load->SCNT = 0;
	CPU_Load->WCNT = 0;
	CPU_Load->Updated = 0;
	
	/* Return DWT counter enabled status */
	return GENERAL_DWTCounterEnable();
}

uint8_t CPULOAD_GoToSleepMode(CPULOAD_t* CPU_Load) {
	uint32_t t;
	static uint32_t l = 0;
	static uint32_t WorkingTime = 0;
	static uint32_t SleepingTime = 0;
	uint8_t irq_status;
	
	/* Add to working time */
	WorkingTime += DWT->CYCCNT - l;
	
	/* Save count cycle time */
	t = DWT->CYCCNT;
	
	/* Get interrupt status */
	irq_status = __get_PRIMASK();
	
	/* Disable interrupts */
	__disable_irq();
	
	/* Go to sleep mode */
	/* Wait for wake up interrupt, systick can do it too */
	__WFI();
	
	/* Increase number of sleeping time in CPU cycles */
	SleepingTime += DWT->CYCCNT - t;
	
	/* Save current time to get number of working CPU cycles */
	l = DWT->CYCCNT;
	
	/* Enable interrupts, process/execute an interrupt routine which wake up CPU */
	if (!irq_status) {
		__enable_irq();
	}
	
	/* Reset flag */
	CPU_Load->Updated = 0;
	
	/* Every 1000ms print CPU load via USART */
	if ((SleepingTime + WorkingTime) >= HAL_RCC_GetHCLKFreq()) {
		/* Save values */
		CPU_Load->SCNT = SleepingTime;
		CPU_Load->WCNT = WorkingTime;
		CPU_Load->Load = ((float)WorkingTime / (float)(SleepingTime + WorkingTime) * 100);
		CPU_Load->Updated = 1;
		
		/* Reset time */
		SleepingTime = 0;
		WorkingTime = 0;
	}
	
	/* Return updated status */
	return CPU_Load->Updated;
}

uint8_t GENERAL_DWTCounterEnable(void) {
    uint32_t c;

    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;


    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;

    /* Reset counter */
    DWT->CYCCNT = 0;

    /* Check if DWT has started */
    c = DWT->CYCCNT;

    /* 2 dummys */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Return difference, if result is zero, DWT has not started */
    return (DWT->CYCCNT - c);
}
