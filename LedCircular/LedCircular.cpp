//LedCircular.cpp  Hardware 16 LED array
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "LedCircular.h"

// default constructor
LedCircular::LedCircular()
{
} //LedCircular

// default destructor
LedCircular::~LedCircular()
{
} //~LedCircular

void LedCircular::select(unsigned char value)
{
	led_c_ =  1 << value;
}

void LedCircular::fill(unsigned char value)
{
	led_c_ = (1 << (value + 1)) - 1;
}

void LedCircular::setSegment(unsigned char segment, bool led_on)
{
	if(led_on==true)
	{
		bitSet(led_c_,segment);
	}
	else
	{
		bitClear(led_c_,segment);
	}
}
bool LedCircular::getSegment(unsigned char segment)
{
	return (bool)bitRead(led_c_,segment);
}
void LedCircular::setFlashOffTicks(unsigned char segment, unsigned char offticks)
{
	flashOnOffTicks_c_[segment] = compressFourBit(flashOnOffTicks_c_[segment],offticks,0);
}
unsigned char LedCircular::getFlashOffTicks(unsigned char segment)
{
	return uncompressFourBit(flashOnOffTicks_c_[segment],0);
}
void LedCircular::setFlashOnTicks(unsigned char segment, unsigned char onticks)
{
	flashOnOffTicks_c_[segment] = compressFourBit(flashOnOffTicks_c_[segment],onticks,1);
}
unsigned char LedCircular::getFlashOnticks(unsigned char segment)
{
	return uncompressFourBit(flashOnOffTicks_c_[segment],1);
}
void LedCircular::flash(unsigned char segment, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn)
{
	flashCnt_[segment] = flashes;
	flashTick_[segment] = 0;
	setFlashOnTicks(segment,onTicks);
	setFlashOffTicks(segment,offTicks);
	setSegment(segment,startOn);
}
void LedCircular::flashStop(unsigned char segment)
{
	flashCnt_[segment] = 0;
	setSegment(segment,false);
}
bool LedCircular::refreshFlash(unsigned char tick_inc)
{
	unsigned char j;
	bool state;
	bool flashing = false;
	unsigned char onticks = 0;
	unsigned char offticks = 0;
	
	for(j=0;j<16;++j)
	{
		if(flashCnt_[j]>0)
		{
			flashing = true;
			flashTick_[j] += tick_inc;
			state = getSegment(j);
			onticks = getFlashOnticks(j);
			offticks = getFlashOffTicks(j);
			if((state==true && flashTick_[j]>=onticks) || (state==false && flashTick_[j]>=offticks))
			{
				if(state==true)
				{
					flashTick_[j] -= onticks;
				}
				else
				{
					flashTick_[j] -= offticks;
				}
				setSegment(j,!state);
				if(flashCnt_[j]<255)
				{
					flashCnt_[j]--;
				}
			}
		}
	}	
	return flashing;
}