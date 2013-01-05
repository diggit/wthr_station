/***************************************************************************
 * Desc: Program for controller Atmega8, it will be placed near sensors,
 * reads raw values, converts them and on masters request, sends them back over UART
 * USED sensors are: temperature and pressure digital i2c sensor BMP085
 * humidity digital sensor DHT11
 * 
 * Author: Patrik Bachan
 * Contact:patrikbachan@gmail.com (email), diggit@jabbim.cz (jabber)
 * 
 * License: GNU GPL v3
 ***************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include "display_ex.c" //functions for more elegant display output
#include <avr/wdt.h>

#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {}

#define FOSC 16000000UL
#define BAUD 19200UL
#define UA_RX_INT_EN 1 //enable UART recieve INT

#include "uart_lib.c"

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
//DHT start sequence
#define DHT_START(b) bit_set(DDRC,b); bit_clr(PORTC,b); delay_p(1000*20); bit_set(PORTC,b); delay_p(4*10); bit_clr(DDRC,b); //sholdn't be PORTC bit 2 cleared too?

//resolution of BMP085 pressure measurement
#define resolution 3 //0 fastest - worst, 3 slowest - best

char str_buffer[]="XXXXXXXXXXX\0";

int8_t response[5]={0,0,0,0,0};//init with zeros, buffer for DHT11

void delay(uint16_t num) ;
int32_t calc_temp();
void getCV();
long calc_pressure();
uint32_t readNB(uint8_t address, uint8_t bytes);
char *itos(int32_t num);
void DHT(uint8_t bit);

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


void delay_p(unsigned long delay)//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
{
	delay=delay<<1;//multiply by 2 to get 1uS delay
	while(delay--) {asm volatile("nop");asm volatile("nop");}
};//dont try to optimize, it's calibrated! (gcc -O2)




int main (void)
{
	uart_init();//init UART
	uart_puts("booted\n");
	i2c_init();//bring up I2C
	
	getCV();//download callibration values from BMP085
	calc_temp();//initial reading
	calc_pressure();
	
	set_sleep_mode(SLEEP_MODE_IDLE); //choose most suitable sleep mode - IDLE (still running UART)
	sleep_enable();//enable sleeping
	
	
	sei(); //enable interrupt, mainly for UART RX

	uart_puts("starting\n");
	while(1)//sleep it after interrupt again and again
	{
		uart_puts("sleeping\n");//debug
		sleep_enable();
		sleep_cpu();//MCU is awaken on every interrupt, this will sleep it back again...
	}	
	
	return 0;
}

void DHT(uint8_t bit)
{
	
	uint8_t i,j;
	
	//"say hello"
	DHT_START(1<<bit);
	//expect ACK
	while(PINC&(1<<bit));//wait for pull down
	while(!(PINC&(1<<bit)));//wait for pull up
	while(PINC&(1<<bit));//wait for pull down
	//init complete
	//link is onw pulled down by DHT
	
	//recieve 40(1<<bit)s
	for(j=0;j<5;j++)
	{
		response[j]=0;//erase old value
		for(i=0;i<8;i++)
		{
			response[j]=response[j]<<1;//shift left - prepare for new (1<<bit)
			while(!(PINC&(1<<bit)));//wait for pull up
			delay_p(50);
			if(PINC&(1<<bit))//i still pulled up>longer pulse>LOG1
			{
				response[j]|=1;//set lowest (1<<bit)
				while(PINC&(1<<bit));//wait for pull down
			}
			//else //already pulled down > shorter pulse > LOG0 , zero is hopefuly there (default)

			
		}
	}
	//link is down
	//end event check
	while(!(PINC&(1<<bit)));//wait for pull up
	//delay_p(20000);//after some time, it should remain pulled up, still
	//if(PINC&2){uart_puts("yeah");}
	//else{uart_puts("noooo");}
	
}

ISR(USART_RXC_vect)//return measured values on request, otherwivise, node will be in idle mode, (maybe sleeping - should be implemented)
{
	cli();
	char reply=UDR;
	if(reply=='Q')//send data as response fo Query
	{
		uart_puts("!,MULTISNS1,T0,");//some ID
		uart_puts(itos(calc_temp()));
		uart_puts(",P0,");
		uart_puts(itos(calc_pressure()));
		uart_puts(",H0,");
		DHT(0);
		uart_puts(itos(response[0]));
		
		uart_puts(",H1,");
		DHT(1);
		uart_puts(itos(response[0]));
		uart_puts(",$\n");
		
	}
	else if (reply=='R')
	{
		uart_puts("Resetting...");
		Reset_AVR();
	}
	else uart_puts("send Q to get data or R for reset\n");
	sei();
}

void getCV()//downloads Calibration Values from sensor
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

uint32_t readNB(uint8_t reg_addr, uint8_t bytes)//reads 'bytes' bytes (up to 4) from address defined by RA/WA beginning on given register address 
{
	uint8_t raw[]={0,0,0,0};//maximal 4 bytes to read
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
	i2c_stop();//end i2c communication
	
	for(i=0;i<=bytes;i++)//merge all recieved data into single value
	{
		sum|=((uint32_t)raw[i])<<(8*(bytes-i));
	}
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
	
	//converting, this magic formula was copied from sensors datasheet
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

char *itos(int32_t num)
{		
	uint8_t index=0;
	
	if(num<0)
	{
		num*=-1;//make it positive
		str_buffer[index]='-';
		index++;
	}
	/*
	else
	{
		str_buffer[0]='+';//positive means no sign
		index++;
	}*/
	
	if(num==0)
	{
		str_buffer[index]='0';
		str_buffer[index+1]='\0';
	}
	else{
		uint32_t divider=1000000000;
		while(divider>0)
		{
			if(num>divider)
			{
				str_buffer[index]='0'+(num/divider)%10;
				index++;
			}
			
			divider/=10;//divide divider by 10, for next calcs.
			
		}
		if(index<10)
		{str_buffer[index]='\0';}
	}
	return str_buffer;
}
