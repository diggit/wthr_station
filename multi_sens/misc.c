#include "misc.h"

int32_t reduce(int32_t value, uint8_t decrease) //correct rounding, not just cropping
{
	uint16_t dec=1;
	for(;decrease>1;decrease--)//calculate divider
		dec*=10;

	if (value/dec%10 > 4)//round
		return value/(10*dec)+1;
	else
		return value/(10*dec);
}

char* itoa_dec_fill(int32_t val,int min,char filler,int decimals)//coverts number to string with defined base and minimal digit, char fillers
{
	static char buf[32] = {0};
	int i = 30;

	if (val==0)
	{
		buf[i--]='0';
	}

	else
	{
		if(val<0)
		{
			val=-val;
			buf[i--]='-';
		}
		
		for (; (val && i); --i, val /= 10)
		{
			if(30-i==decimals && decimals!=0 )
				buf[i--]='.';

			buf[i] = "0123456789abcdef"[val % 10];
		}

	}

	if(30-i<=decimals)
	{
		for(;30-i<decimals;)
			buf[i--]='0';
		buf[i--]='.';
		buf[i--]='0';
	}

	for (; min+i>30; --i)
		buf[i] = filler;

	return &buf[i+1];
}

char* itoa_fill(int32_t val, int min, char filler)
{
	return itoa_dec_fill(val,min,filler,0);
}

char* itoa(int32_t val, int min)
{
	return itoa_dec_fill(val,min,' ',0);
}

char* itoa_dec(int32_t val,int min,int decimals)//coverts number to string with defined base and minimal digit, char fillers
{
	return itoa_dec_fill(val,min,' ',decimals);
}
