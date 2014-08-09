#include "calibrated_loop.h"

void delay_us(unsigned long delay)//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
{
	delay=delay<<1;//multiply by 2 to get 1uS delay
	while(delay--) {NOP;NOP;}
};

void delay_ms(uint16_t ms)
{
	for(;ms>0;ms--)
		delay_us(1000);
}

uint8_t wait_timeout(volatile uint8_t *port, uint8_t mask, uint8_t timeout, uint8_t high)
{
	uint8_t counter;
	for(counter=0;
		((!(*port & mask) && high ) || //high level stop
		((*port & mask) && !high )) //low level stop
		&& counter!=timeout ;counter++)

		NOP;
	if(counter==timeout)
	{
		return 1;
		#ifdef debug
		uart_printl("STH timed out!");
		#endif
	}
	else
		return 0;
}

uint8_t wait_timeout_simple(volatile uint8_t *port, uint8_t mask)
{
	return wait_timeout(port,mask,255,1);
}
