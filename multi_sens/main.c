/***************************************************************************
 * Desc: Program for controller Atmega8, it will be placed near sensors,
 * reads raw values, converts them a after request, sends them back over UART
 * USED sensors are: temperature and pressure digital i2c sensor BMP085
 * humidity digital sensor DHT11
 * 
 * Author: Patrik Bachan
 * Contact:patrikbachan@gmail.com (email), diggit@jabbim.cz (jabber)
 * 
 * License: GNU GPL v3
 ***************************************************************************/
#include <avr/io.h>
#include "display_ex.c" //functions for more elegant display output


//basic macros for simple bit writing
#define bit_set(p,m) ((p) |= (m))
#define bit_clr(p,m) ((p) &= ~(m))

//i2c library for communication
#include "i2cmaster.c"

//addresses of i2c sensor
#define RA 0b11101111
#define WA 0b11101110

//waiting for signal that indicates End Of Conversion on pin PB0
#define WFC(); while( !(PINB & 1) ); //Wait For Conversion

#define resolution 3 //0 fastest - worst, 3 slowest - best

void delay(uint16_t num) ;
int32_t calc_temp();
void getCV();
long calc_pressure();
uint32_t readNB(uint8_t address, uint8_t bytes);

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
	uint32_t b7;
	int32_t b6;
	int32_t b5;
	uint32_t b4;
	int32_t b3;
} CVC;



void example_run() //temporary, only for testing of calculations, compare with datasheet
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
	
	uint32_t tlak,temp;
	log_str("pressure:");
	while(1)
	{
		//log_str("temperature:");
		//log_num(calc_temp());
		temp=calc_temp();
		//log_num(987654);
		
		tlak=calc_pressure();
		log_str(itos(tlak));
		log_str(itos(temp));
		//log_str(itos(-1234567890)); //just for negative value testing purposes
		delay(10000);
	}	
	
	return 0;
}

void getCV()
{
	CV.ac1 = readNB(0xAA,2);
	CV.ac2 = readNB(0xAC,2);
	CV.ac3 = readNB(0xAE,2);
	CV.ac4 = readNB(0xB0,2);
	CV.ac5 = readNB(0xB2,2);
	CV.ac6 = readNB(0xB4,2);
	CV.b1 = readNB(0xB6,2);
	CV.b2 = readNB(0xB8,2);
	CV.mb = readNB(0xBA,2);
	CV.mc = readNB(0xBC,2);
	CV.md = readNB(0xBE,2);
}



uint32_t readNB(uint8_t reg_addr, uint8_t bytes)
{
	uint8_t raw[]={0,0,0,0};//moximal 4 bytes to read
	uint32_t sum=0;
	uint8_t i;//some index var.
	if(bytes>4){bytes=4;} //higher vals. shrink to 4
	bytes--;//now, our values are 0-3, better suits here
	
	i2c_start(WA);//we wannay say what to read, that is writing...
	i2c_write(reg_addr);//read from given addr
	i2c_rep_start(RA);//restart i2c comm.
	
	for(i=0;i<bytes;i++)
	{
		raw[i]=i2c_read(1);
	}
	raw[i]=i2c_read(0);//this is out last byte
	i2c_stop();
	
	for(i=0;i<=bytes;i++)
	{
		sum|=((uint32_t)raw[i])<<(8*(bytes-i));
	}
	//sum=((((uint32_t)raw[0])<<16)|(((uint32_t)raw[1])<<8)|(uint32_t)raw[2]);
	return sum;
	
}







int32_t calc_temp()//measures raw value, converts it into human readable value and returns it
{
	uint8_t err=0;//err test, only for debug purposes, won't be used
	int32_t X1,X2;//buffers for translate vals into human readable
	
	//proceed temp mesurement 
	i2c_start(WA);
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x2E);//and initiate temp mesurement
	i2c_stop();
	
	WFC();//wait for EOC
	
	//read raw temp value
	uint16_t UT=(uint16_t)readNB(0xF6,2);
	
	//converting, this magic formula is copied from sensors datasheet
	X1=(((int32_t)UT-(int32_t)CV.ac6)*(int32_t)CV.ac5)>>15;
	X2=((int32_t)CV.mc<<11)/(X1+(int32_t)CV.md);
	CVC.b5=X1+X2;
	return((CVC.b5+8)>>4);
}

long calc_pressure()//similar to calc_temp, but works with pressure
{
	uint8_t err=0;
	int32_t X1,X2,X3,p;
	uint32_t raw_p;
	
	i2c_start(WA);
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x34+(resolution<<6));//and measure pressure
	i2c_stop();
	
	WFC();//wait for EOC
	
	/*
	//MUST BE MOVED AWAY into func.
	
	uint8_t raw_L=0,raw_H=0,raw_X=0;
	
	
	i2c_start(WA);//we wannay say what to read, that is writing... (OMG, silly mistake)
	i2c_write(0xF6);//read from given addr
	i2c_rep_start(RA);//restart i2c comm.
	raw_H=i2c_read(1);//read byte and will be next
	raw_L=i2c_read(1);//read byte and will be next
	raw_X=i2c_read(0);//read byte and no next
	i2c_stop();//stop
	raw_p=( ((uint32_t) raw_H << 16) | ((uint32_t) raw_L << 8) | ((uint32_t) raw_X ) )>>(8-resolution); //merge all 3 bytes*/
	raw_p=(readNB(0xF6,3))>>(8-resolution);
	

	//example_run();
	//raw_p=23843;
	
	//main calculation, formula also copied from sensors datasheet
	CVC.b6=CVC.b5-4000; //this affects temp. dependency
	X1=(CV.b2 * ((CVC.b6*CVC.b6)>>12))>>16;
	X2=(CV.ac2 * CVC.b6)>>11;
	X3=X1 + X2;
	CVC.b3=(((((uint32_t) CV.ac1) * 4 +X3)<<resolution) + 2)>>2;
	
	X1=(CV.ac3 * CVC.b6)>>13;
	X2=(CV.b1 * ((CVC.b6*CVC.b6)>>12))>>16;
	X3=((X1 + X2) + 2)>>2;
	CVC.b4=(CV.ac4 * (uint32_t)(X3 + 32768))>>15;
	
	CVC.b7=((uint32_t)(raw_p - CVC.b3) * (50000>>resolution));
	if (CVC.b7 < 0x80000000)	{p=(CVC.b7<<1) / CVC.b4;}
	else						{p=(CVC.b7 / CVC.b4)<<1;}
	
	X1=(p>>8)*(p>>8);
	X1=(X1 * 3038)>>16;
	X2=(-7357 * p)>>16;
	p+=(X1 + X2 + 3791)>>4;//original
	p+=3400;//my sensor has wrong value at output, offset added to fix it, I hope, It'll work ;-)

	return p;
}



void delay(uint16_t num) //simple delay loop, really stupid but works
{
	uint16_t i,j;
	for (i = 0; i < num; i++)
		for (j=0;j<100;j++)
		;
}
