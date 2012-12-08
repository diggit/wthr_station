#ifndef FOSC
	
#define FOSC 16000000UL

#endif

#ifndef BAUD

#define BAUD 9600UL

#endif

#define ubrr_val (FOSC/(16*BAUD)-1)

#ifndef UA_RX_INT_EN

#define UA_RX_INT_EN 0

#endif

unsigned char uart_getc( void );
void uart_putc(char data );
void uart_puts(char *str);
void uart_init(void);
