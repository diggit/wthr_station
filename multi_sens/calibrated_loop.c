#ifndef loop_lib
	#define loop_lib
	
#ifndef _NOP
	#define _NOP asm volatile("NOP")
#endif
void delay_us(unsigned long delay)//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
{
	delay=delay<<1;//multiply by 2 to get 1uS delay
	while(delay--) {_NOP;_NOP;}
};

#endif
