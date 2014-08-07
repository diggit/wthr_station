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


uint8_t DHT_response[5]={0,0,0,0,0};//init with zeros, buffer for DHT11


//TODO: rewrite infinite waiting to timed out one
void DHT(uint8_t bit)
{
	
	uint8_t i,j;

	//"say hello"
	DHT_START(1<<bit);
	//expect ACK
	while(DHTI&(1<<bit));//wait for pull down
	
	while(!(DHTI&(1<<bit)));//wait for pull up
	while(DHTI&(1<<bit));//wait for pull down

	
	//recieve 40(1<<bit)s
	for(j=0;j<5;j++)
	{
		DHT_response[j]=0;//erase old value
		for(i=0;i<8;i++)
		{
			DHT_response[j]=DHT_response[j]<<1;//shift left - prepare for new (1<<bit)
			while(!(DHTI&(1<<bit)));//wait for pull up
			delay_us(50);
			if(DHTI&(1<<bit))//i still pulled up>longer pulse>LOG1
			{
				DHT_response[j]|=1;//set lowest (1<<bit)
				while(DHTI&(1<<bit));//wait for pull down
			}
			//else //already pulled down > shorter pulse > LOG0 , zero is hopefuly there (default)
		}
	}
	//link is down
	//end event check
	while(!(DHTI&(1<<bit)));//wait for pull up
}