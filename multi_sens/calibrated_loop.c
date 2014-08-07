#include "calibrated_loop.h"

void delay_us(unsigned long delay)//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
{
	delay=delay<<1;//multiply by 2 to get 1uS delay
	while(delay--) {_NOP;_NOP;}
};

void delay_ms(uint16_t ms)
{
	for(;ms>0;ms--)
		delay_us(1000);
}
