#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;

void SysTick_Handler()
{
	TimingDelay_Decrement();
}

void init_GPIO()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int i;
int main(){
	
	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}
	init_GPIO();
	for(i = 0; i < 2; i++)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		delay_ms(200);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		delay_ms(100);
	}	

	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		}
	 }
}
