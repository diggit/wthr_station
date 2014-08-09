// Copyright (C) 2014  xorly

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "dht.h"


uint8_t DHT_response[4]={0,0,0,0};//init with zeros, buffer for DHT11


void DHT_start(uint8_t pinmask)//tell DHT, to send data
{
	bit_set(DHTD,pinmask);
	delay_us(1000*30);
	bit_clr(DHTD,pinmask);
	delay_us(4*10);
}

uint8_t DHT_rx_byte(uint8_t *storage,uint8_t pinmask)
{
	*storage=0;
	for(uint8_t i=0;i<8;i++)
	{
		*storage<<=1;//shift left - prepare for new bit
		if(wait_timeout(&DHTI,pinmask,255,1))//wait for pull up
			return 1;
		delay_us(50);
		if(DHTI&pinmask)//if still pulled up>longer pulse>LOG1
		{
			*storage|=1;//set lowest pinmask
			if(wait_timeout(&DHTI,pinmask,255,0))//wait for pull down
				return 1;
		}
		//else //already pulled down > shorter pulse > LOG0 , zero is hopefuly there (default)
	}
	return 0;
}

uint8_t DHT(uint8_t bit)
{
	uint8_t pinmask=(1<<bit);
	uint8_t checksum_calc=0;
	uint8_t checksum_rx;

	for (int i = 0; i < DHT_retry_count; ++i)
	{
		//"say hello"
		DHT_start(pinmask);
		//expect ACK
		if(wait_timeout(&DHTI,pinmask,255,0))//wait for pull down
			return 1;

		if(wait_timeout(&DHTI,pinmask,255,1))//wait for pull up
			return 1;
		if(wait_timeout(&DHTI,pinmask,255,0))//wait for pull down
			return 1;
		
		//recieve 40 bits (2 bytes humidity, 2 bytes temterature, 1 byte checksum)
		for(uint8_t i=0;i<4;i++)
		{
			if(DHT_rx_byte(&DHT_response[i],pinmask))
				return 1;
			checksum_calc+=DHT_response[i];
		}

		DHT_rx_byte(&checksum_rx,pinmask);
		//end event check
		if(wait_timeout(&DHTI,pinmask,255,1))//wait for pull up
			return 1;

		#ifdef debug
		uart_print("DHT checksum ");
		#endif
		if(checksum_rx==checksum_calc)
		{
			#ifdef debug
			uart_println("OK");
			#endif
			return 0;
		}
		else
		{
			#ifdef debug
			uart_println("FAIL");
			#endif	
		}
	}
	return 1;
}
