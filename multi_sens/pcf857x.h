/*
 *
 * Author:         Lukas Otava, http://lukaso.iglu.cz
 * Description:    Library for PCF857x based LCDs
 *
 */
#define TWI_INIT() PORTC |= 0b110000;  TWBR = 52 // 52 je konstanta pro rychlost ~100kHz pøi 12MHz

#define STD_OUT
uint8_t i;
#define CLEAN() for(i = 96; i !=0 ; i--) znak(' '); pos_x = 0; pos_y = 0

#define GOTO(x,y) pos_x = (x); pos_y = (y)

uint8_t pos_x, pos_y;

void TwI_start(void);
void TWI_write(uint8_t value);
void TWI_stop(void);

void lcdinit(void);

void znak_ (uint8_t chip, uint8_t row, uint8_t col, char znak);
void znak__(uint8_t chip, uint8_t row, uint8_t col, char znak);
void znak(char znak);
void lcd_str( char *text );

#ifdef STD_OUT
#include <stdio.h>
static int LCD_putchar(char zapsat, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(LCD_putchar, NULL, _FDEV_SETUP_WRITE);
#endif
