/*
 *
 * Author:         Lukas Otava, http://lukaso.iglu.cz
 * Description:    Library for PCF857x based LCDs
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "pcf857x.h"

#include "char5x7.h"
#define FNT char5x7

void TwI_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}
void TWI_write(uint8_t value)
{
	TWDR = value;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}
void TWI_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void lcdinit(void)
{
	TWI_INIT();

	#ifdef STD_OUT
	stdout = &mystdout;
	#endif

	pos_x = 0;
	pos_y = 0;

	TwI_start();
	TWI_write(0b01111000);// wr adr
	TWI_write(0b11000100);// disp en, mi mode
	TWI_write(0b01111100);// bank sel
	TWI_stop();
}

void znak_ (uint8_t chip, uint8_t row, uint8_t col, char znak)
{
	znak -= 0x20;
	uint16_t bit = znak*5;
	TwI_start();
	TWI_write(0b01111000);// wr adr
	TWI_write(0b11100000 | chip);// sub - adr
	TWI_write(0b11110000 | row) ;// ram access ROW
	TWI_write(col*5);
	TWI_write(pgm_read_byte(&FNT[bit++]));
	TWI_write(pgm_read_byte(&FNT[bit++]));
	TWI_write(pgm_read_byte(&FNT[bit++]));
	TWI_write(pgm_read_byte(&FNT[bit++]));
	TWI_write(pgm_read_byte(&FNT[bit++]));
	TWI_stop();
}
void znak__(uint8_t chip, uint8_t row, uint8_t col, char znak)
{
	znak -= 0x20;
	col = 7-col;
	uint16_t bit = znak*5 +4;
	TwI_start();
	TWI_write(0b01111000);// wr adr
	TWI_write(0b11100000 | chip);// sub - adr
	TWI_write(0b11110000 | row) ;// ram access ROW
	TWI_write(col*5);
	TWI_write(pgm_read_byte(&FNT[bit--]));
	TWI_write(pgm_read_byte(&FNT[bit--]));
	TWI_write(pgm_read_byte(&FNT[bit--]));
	TWI_write(pgm_read_byte(&FNT[bit--]));
	TWI_write(pgm_read_byte(&FNT[bit--]));
	TWI_stop();
}
void znak(char znak)
{
	if (pos_x<8)
		znak_(0,pos_y,pos_x,znak);
		else
			if (pos_x>15)
				znak_(2,pos_y,pos_x-16,znak);
				else
					znak__(1,pos_y,pos_x-8,znak);
	if (pos_x>22)
	{
		pos_x= 0;
		if (pos_y<3)
			pos_y++;
			else
				pos_y = 0;
	} else
		pos_x++;
}
void lcd_str( char *text )
{
	do
	{
		if (*text == '\n')
			{pos_x=0;pos_y++;text++;}
		znak(*text);
		text++;
	} while((*text)!=0);
}

#ifdef STD_OUT
int LCD_putchar(char zapsat, FILE *stream)
{
	if (zapsat == '\n')
		{pos_x=0;pos_y++;}
	else znak(zapsat);
    return 0;
}
/// !!! stdout = &mystdout; //do main
#endif
