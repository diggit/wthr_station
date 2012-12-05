#include <inttypes.h>


#include "dgtslib.h"

/*
	input is int which is converted into char array ("=string")
	teoretical maximum i 65535/2, so 5  chars max
*/
void itos(int16_t input, unsigned short min_chars,char replace,uint8_t w_sign,char *aray)
{
	
	//sum of digits
	uint8_t digits=1,offset=0;
	char sign;
	if(input<0)
	{
		sign='-';
		input=-input;
	}
	else
		sign='+';
	
	
	if (input>9999)
		digits=5;
	else if (input>999)
		digits=4;
	else if (input>99)
		digits=3;
	else if (input>9)
		digits=2;
	
	if (min_chars>digits)
	{
		offset=min_chars-digits;
	}
	else if (w_sign)
		offset=1;//sign char reservation


	char output[256];
	if (offset)
		for(i=0;i<offset;i++)
			output[i]=replace;
	if (w_sign)
			output[offset-1]=sign;
	
	if (input>=10000)
		output[offset]='0'+input/10000;
	else if( min_chars==5)
		output[offset]=replace;
	if (input>=1000)
		output[1+offset]='0'+input/1000%10;
	else if( min_chars==4)
		output[1+offset]=replace;
	if (input>=100)
		output[2+offset]='0'+input/100%10;
	else if( min_chars==3)
		output[2+offset]=replace;
	if (input>=10)
		output[3+offset]='0'+input/10%10;
	else if( min_chars==2)
		output[3+offset]=replace;
	output[4+offset]='0'+input%10;
	
	for (i=0;i<offset+digits;i++)
		aray[i]='p';
}