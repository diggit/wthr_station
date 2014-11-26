#ifndef __MISC_LIB
	#define __MISC_LIB

	#include <inttypes.h>
	#include <avr/wdt.h>

	#define uabs(x)  ( ( (x) < 0) ? -(x) : (x) )

	//global macros for bitwise manipulation
	#define bit_get(p,m) ((p) & (m))
	#define bit_set(p,m) ((p) |= (m))
	#define bit_clr(p,m) ((p) &= ~(m))
	#define bit_flip(p,m) ((p) ^= (m))
	#define bit_con(c,p,m) (c ? bit_set(p,m) : bit_clr(p,m)) //macro for conditional bit set/clr
	#define BIT(x) (0x01 << (x))
	#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

	//simple NOP, belongs to empty loops etc.
	#define NOP __asm__ __volatile__ ("NOP");

	#define array_length(A) sizeof(A)/sizeof(A[0])


	int32_t	reduce(int32_t value, uint8_t decrease);
	char* itoa_dec_fill(int32_t val,int min,char filler,int decimals);
	char* itoa_fill(int32_t val, int min, char filler);
	char* itoa(int32_t val, int min);
	char* itoa_dec(int32_t val,int min,int decimals);

	#ifdef debug
	#define Reset_AVR() uart_println("reset called!"); wdt_enable(WDTO_30MS); while(1) {NOP;}//little workaround, atmega has no software reset
	#else
	#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {NOP;}//little workaround, atmega has no software reset
	#endif


	#define len(x)  (sizeof(x) / sizeof(x[0]))



#endif
