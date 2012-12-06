/*
 *
 * Author:         Lukas Otava, http://lukaso.iglu.cz
 * Description:    Library for PCF857x based LCDs
 *
 */
#include <avr/io.h>
#include <string.h>
#include "pcf857x.c"

#define bit_set(p,m) ((p) |= (m))
#define bit_clr(p,m) ((p) &= ~(m))

//pokus s i2c cidlem...
#include "i2cmaster.c"

#define log_okMSG() log_MSG('Y')
#define log_failMSG() log_MSG('F')
#define log_unkMSG() log_MSG('?')

#define log_str(p) log_str_ex(p,1)
#define log_strn(p) log_str_ex(p,0)

#define log_num(p) log_num_ex(p,1)
#define log_numn(p) log_num_ex(p,0)

#define RA 0b11101111
#define WA 0b11101110

void log_str_ex(char *text,char NL);
void log_redraw();
void delay(unsigned int num) ;
void log_busyMSG(char text[]);
void log_bar(short per);
long calc_temp();
void log_MSG(char status);
void itos(unsigned int num, char *output);
void log_num_ex(int number,char tmp);
int getW(char reg_addr);
char getB(char reg_addr);
void getCV();

struct {
	int16_t ac1;
	int16_t ac2; 
	int16_t ac3; 
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1; 
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
}CV;




char DSP[4][24]={{""},{""},{""},{""}};
uint8_t busyMSG=0,bar=0;


int main (void)
{

	uint8_t i;
	lcdinit();
	CLEAN();
	log_str("starting...");

	log_busyMSG("reading calib. vals");
	getCV();
	log_okMSG();
	log_str("real temp");
	//log_num(calc_temp());
	while(1)
	{
		//log_str("real");
		log_numn(calc_temp());
		delay(5000);
	}	
	
	return 0;
}

void getCV()
{
	CV.ac1 = getW(0xAA);
	CV.ac2 = getW(0xAC);
	CV.ac3 = getW(0xAE);
	CV.ac4 = getW(0xB0);
	CV.ac5 = getW(0xB2);
	CV.ac6 = getW(0xB4);
	CV.b1 = getW(0xB6);
	CV.b2 = getW(0xB8);
	CV.mb = getW(0xBA);
	CV.mc = getW(0xBC);
	CV.md = getW(0xBE);
}

char getB(char reg_addr)
{
	uint8_t raw;
	i2c_start(WA);//we wannay say what to read, that is writing... (OMG, silly mistake)
	i2c_write(reg_addr);//read from given addr
	i2c_rep_start(RA);//restart i2c comm.
	raw=i2c_read(0);//read byte and no next
	i2c_stop();//stop
	return raw;
}

int getW(char reg_addr)
{
	uint8_t raw_L,raw_H;
	i2c_start(WA);//we wannay say what to read, that is writing... (OMG, silly mistake)
	i2c_write(reg_addr);//read from given addr
	i2c_rep_start(RA);//restart i2c comm.
	raw_H=i2c_read(1);//read byte and will be next
	raw_L=i2c_read(0);//read byte and no next
	i2c_stop();//stop
	return (raw_H<<8)|raw_L;
}

void log_num_ex(int number,char tmp)
{
	char buff[]="QQQQQ";
	itos(number,buff);
	log_str_ex(buff,tmp);
}


void itos(unsigned int num, char *output)
{
	//char output[]="12345";
	
	if (num>=10000)
		output[0]='0'+num/10000;
	else
		output[0]=' ';
		
	if (num>=1000)
		output[1]='0'+num/1000%10;
	else
		output[1]=' ';
	
	if (num>=100)
		output[2]='0'+num/100%10;
	else
		output[2]=' ';
	
	if (num>=10)
		output[3]='0'+num/10%10;
	else
		output[3]=' ';
	
	if (num>=1)
		output[4]='0'+num%10;
	else
		output[4]='X';
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


long calc_temp()
{
	char err=0;//err test
	long X1,X2;//buffers for translate vals into human readable
	
	//measure temp
	//log_busyMSG("start measuring");
	i2c_start(WA);
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x2E);//and measure temp
	//if(err)	{log_failMSG();}
	//else	{log_okMSG();}
	i2c_stop();
	
	
	delay(100);//wait for complete measurement
	
	//read raw temp value
	uint16_t UT=getW(0xF6);
	//converting
	
	//log_str("raw");
	//log_num(UT);
	
	X1=(((long)UT-(long)CV.ac6)*(long)CV.ac5)>>15;
	X2=((long)CV.mc<<11)/(X1+(long)CV.md);
	return((X1+X2+8)>>4);
}

void log_bar(short per)
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

void log_str_ex(char* text,char NL)
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

void delay(unsigned int num) //simple delay loop
{
	unsigned int i,j;
	for (i = 0; i < num; i++)
		for (j=0;j<100;j++)
		;
}
