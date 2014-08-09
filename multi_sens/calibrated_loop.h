#ifndef _LOOP_LIB
	#define _LOOP_LIB


	#include <inttypes.h>
	#include "misc.h"

	#define delay delay_ms

	void delay_ms(uint16_t ms);
	void delay_us(unsigned long delay);//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
	uint8_t wait_timeout(volatile uint8_t *port, uint8_t mask, uint8_t timeout, uint8_t high);
	uint8_t wait_timeout_simple(volatile uint8_t *port, uint8_t mask);

#endif
