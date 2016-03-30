#include <delay.h>

void SysTick_Handler()
{
	TimingDelay_Decrement();
}

int main(void) {
	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}
}

