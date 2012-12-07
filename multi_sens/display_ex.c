#include "pcf857x.c"

#include "display_ex.h"



void log_bar(uint8_t per)
{
	uint8_t i,j;
	if(!bar)//bargraph not started, roll to new line
	{
		if(DSP[3][0])//roll only with full display
		{
		for(i=1;i<4;i++)
		for(j=0;j<24;j++)
			DSP[i-1][j]=DSP[i][j];//copy actual to upper line
		}
		
		
	}
	
	
	for(i=0;i<3  && DSP[i][0] ;i++);//find first empty line, var i stay
	if(bar && !DSP[i][0])
		i--;
	bar=1;
		for(j=0;j<20;j++)
		{
			if(per/5>j)
				DSP[i][j]='^';
			else
				DSP[i][j]=' ';
		}
		
		for(j=20;j<24;j++)
			DSP[i][j]=' ';
		
		if(per/100)
			DSP[i][20]=48+per/100;
		if(per/10 )
			DSP[i][21]=48+per%100/10;
		DSP[i][22]=48+per%10;
		DSP[i][23]='\%';
		
	if(per>=100)
		bar=0;
	log_redraw();

	
}

void log_str_ex(char* text,uint8_t NL)
{
	
	uint8_t i,j,mezery=0;
	//shift all lines up
	if((DSP[3][0])&& NL)//roll only with full display and iw we wanna roll..
	{
		for(i=1;i<4;i++)
		for(j=0;j<24;j++)
			DSP[i-1][j]=DSP[i][j];//copy actual to upper line
	}

	
	//fill last line
	for(i=0;i<3  && DSP[i][0] ;i++);//find first empty line, var i stays
		
	for(j=0;j<24;j++)
	{
		if(text[j]=='\0')//after mast char, continue with spaces to the EOL
			mezery=1;
		if(!mezery)
			DSP[i][j]=text[j];
		else
			DSP[i][j]=' ';
	}
	log_redraw();
}

void log_busyMSG(char *text)
{
	uint8_t i,mezery=0;
	char Stext[21];
	char end[3]="[B]";
	if(busyMSG)
		log_unkMSG();
	for(i=0;i<21;i++)
	{
		if(text[i]=='\0')
			mezery=1;
		if(!mezery)
			Stext[i]=text[i];
		else
			Stext[i]=' ';
	}
	for(i=21;i<24;i++)
		Stext[i]=end[i-21];
	log_str(Stext);
	busyMSG=1;
}

void log_redraw()
{
	uint8_t i,j;
	GOTO(0,0);//redraw from 1st char & line
	for(j=0;j<4;j++)
		for(i=0;i<24&&DSP[j][i]!='\0';i++)//redraw all lines, but not continue after empty places
			znak(DSP[j][i]);
}

void log_MSG(char status)
{
	uint8_t i;
	if(busyMSG)
	{
		for(i=3;i>0 && DSP[i][22]!='B';i--);
		if(DSP[i][22]=='B')
			DSP[i][22]=status;
		else
			busyMSG=0;
			
	}
	log_redraw();
	busyMSG=0;
}

void log_num_ex(int32_t number,uint8_t tmp)
{
	char buff[]="QQQQQQ";
	itos(number,buff);
	log_str_ex(buff,tmp);
}

void itos(uint32_t num, char *output)
{
	//char output[]="12345";
	
	if (num>=100000)
		output[0]='0'+num/100000;
	else
		output[0]=' ';
	
	if (num>=10000)
		output[1]='0'+num/10000%10;
	else
		output[1]=' ';
		
	if (num>=1000)
		output[2]='0'+num/1000%10;
	else
		output[2]=' ';
	
	if (num>=100)
		output[3]='0'+num/100%10;
	else
		output[3]=' ';
	
	if (num>=10)
		output[4]='0'+num/10%10;
	else
		output[4]=' ';
	
	if (num>=1)
		output[5]='0'+num%10;
	else
		output[5]='X';
}
