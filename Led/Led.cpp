//Led.cpp  Hardware  LED 
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

#include "Led.h"

// default constructor
Led::Led()
{
} //Led

// default destructor
Led::~Led()
{
} //~Led

void Led::setFlashOffTicks(unsigned char offticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,offticks,0);
}
unsigned char Led::getFlashOffTicks()
{
	return uncompressFourBit(flashOnOffTicks_c_,0);
}
void Led::setFlashOnTicks(unsigned char onticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,onticks,1);
}
unsigned char Led::getFlashOnticks()
{
	return uncompressFourBit(flashOnOffTicks_c_,1);
}
void Led::flash(unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn)
{
	flashCnt_ = flashes;
	flashTick_ = 0;
	setFlashOnTicks(onTicks);
	setFlashOffTicks(offTicks);
	setState(startOn);
}
void Led::flashStop()
{
	flashCnt_ = 0;
	setState(false);
}
bool Led::refreshFlash(unsigned char tick_inc)
{
	if(flashCnt_>0)
	{
		flashTick_ += tick_inc;
		unsigned char onticks = getFlashOnticks();
		unsigned char offticks = getFlashOffTicks();
		if((led_==true && flashTick_>=onticks) || (led_==false && flashTick_>=offticks))
		{
			if(led_==true)
			{
				flashTick_ -= onticks;
			}
			else
			{
				flashTick_ -= offticks;
			}
			led_ = !led_;
			if(flashCnt_<255)
			{
				flashCnt_--;
			}
		}
	}	
}