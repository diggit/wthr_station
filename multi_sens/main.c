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
void delay(uint16_t num) ;
void log_busyMSG(char text[]);
void log_bar(short per);
int32_t calc_temp();
void log_MSG(char status);
void itos(unsigned long num, char *output);
void log_num_ex(long number,char tmp);
int getW(char reg_addr);
char getB(char reg_addr);
void getCV();
long calc_pressure();

struct { //downloaded calibration values
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

struct{//calibration values calculated
	unsigned long b7;
	long b6;
	long b5;
	unsigned long b4;
	long b3;
} CVC;

char DSP[4][24]={{""},{""},{""},{""}};
uint8_t busyMSG=0,bar=0;

const uint8_t resolution=2; //0 fastest - worst, 3 slowest - best


void example_run()
{
	CVC.b5=2399;
	CV.ac1=408;
	CV.ac2=-72;
	CV.ac3=-14383;
	CV.ac4=32741;
	CV.ac5=32757;
	CV.ac6=23153;
	CV.b1=6190;
	CV.b2=4;
	CV.mb=-32767;
	CV.mc=-8711;
	CV.md=2868;
}

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
	
	uint32_t tlak;
	while(1)
	{
		//log_str("temperature:");
		//log_num(calc_temp());
		calc_temp();
		delay(1000);
		log_num(987654);
		log_str("pressure:");
		tlak=calc_pressure();
		log_num(tlak);
		delay(5000);
		//CLEAN();
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

void log_num_ex(long number,char tmp)
{
	char buff[]="QQQQQQ";
	itos(number,buff);
	log_str_ex(buff,tmp);
}


void itos(unsigned long num, char *output)
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


int32_t calc_temp()
{
	char err=0;//err test
	int32_t X1,X2;//buffers for translate vals into human readable
	
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
	
	X1=(((int32_t)UT-(int32_t)CV.ac6)*(int32_t)CV.ac5)>>15;
	X2=((int32_t)CV.mc<<11)/(X1+(int32_t)CV.md);
	CVC.b5=X1+X2;
	return((CVC.b5+8)>>4);
}

long calc_pressure()
{
	char err=0;
	long X1,X2,X3,p;
	unsigned long raw_p;
	
	i2c_start(WA);
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x34+(resolution<<6));//and measure pressure
	i2c_stop();
	
	delay(1000);
	
	
	//MUST BE MOVED AWAY into func.
	
	uint8_t raw_L=0,raw_H=0,raw_X=0;
	
	i2c_start(WA);//we wannay say what to read, that is writing... (OMG, silly mistake)
	i2c_write(0xF6);//read from given addr
	i2c_rep_start(RA);//restart i2c comm.
	raw_H=i2c_read(1);//read byte and will be next
	raw_L=i2c_read(1);//read byte and will be next
	raw_X=i2c_read(0);//read byte and no next
	i2c_stop();//stop
	raw_p=( ((unsigned long) raw_H << 16) | ((unsigned long) raw_L << 8) | ((unsigned long) raw_X ) )>>(8-resolution); //merge all 3 bytes
	

	//example_run();
	//raw_p=23843;
	log_num(raw_p);
	
	delay(1000);
	
	//main calculation
	CVC.b6=CVC.b5-4000;
	X1=(CV.b2 * ((CVC.b6*CVC.b6)>>12))>>16;
	X2=(CV.ac2 * CVC.b6)>>11;
	X3=X1 + X2;
	CVC.b3=(((((unsigned long) CV.ac1) * 4 +X3)<<resolution) + 2)>>2;
	
	X1=(CV.ac3 * CVC.b6)>>13;
	X2=(CV.b1 * ((CVC.b6*CVC.b6)>>12))>>16;
	X3=((X1 + X2) + 2)>>2;
	CVC.b4=(CV.ac4 * (unsigned long)(X3 + 32768))>>15;
	
	CVC.b7=((unsigned long)(raw_p - CVC.b3) * (50000>>resolution));
	if (CVC.b7 < 0x80000000)
	{
		p=(CVC.b7<<1) / CVC.b4;
	}
	else
	{
		p=(CVC.b7 / CVC.b4)<<1;
	}
	
	X1=(p>>8)*(p>>8);
	X1=(X1 * 3038)>>16;
	X2=(-7357 * p)>>16;
	p+=(X1 + X2 + 3791)>>4;

	return p;
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

void delay(uint16_t num) //simple delay loop
{
	uint16_t i,j;
	for (i = 0; i < num; i++)
		for (j=0;j<100;j++)
		;
}
