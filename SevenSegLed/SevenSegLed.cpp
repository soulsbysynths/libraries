//SevenSegLedProgmem.h  7 segment display, using minimum progmem and max speed. written for AT-ARP
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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

#include "SevenSegLed.h"

// default constructor
SevenSegLed::SevenSegLed()
{
} //SevenSegLed

// default destructor
SevenSegLed::~SevenSegLed()
{
} //~SevenSegLed

void SevenSegLed::displayHex(unsigned char newValue)
{
	newValue &= 0x0F;
	if(newValue<0x0A)
	{
		led_charbuf_c_ = pgm_read_word(&(LED_CHARS[newValue + 16]));
	}
	else
	{
		led_charbuf_c_ = pgm_read_word(&(LED_CHARS[newValue + 23]));
	}
	if(visible_)
	{
		led_c_ = led_charbuf_c_ | (led_c_ & 0x0002);
	}
}
void SevenSegLed::displayTapeRoll(unsigned char frame)
{
	frame &= 0x07;
	frame++;
	led_charbuf_c_ = pgm_read_word(&(LED_CHARS[frame]));
	if(visible_)
	{
		led_c_ = led_charbuf_c_ | (led_c_ & 0x0002);
	}
}
void SevenSegLed::displayChar(char character)
{
	character -= 32;
	character &= 0x3F;
	led_charbuf_c_ = pgm_read_word(&(LED_CHARS[character]));
	if(visible_)
	{
		led_c_ = led_charbuf_c_ | (led_c_ & 0x0002);
	}
}
void SevenSegLed::setDp(bool way)
{
	dp_ = way;
	if(dp_!=invertDp_)
	{
		led_c_ &= 0xFFFD;
	}
	else
	{
		led_c_ |= 0x0002;
	}
}
void SevenSegLed::setInvertDp(bool way)
{
	invertDp_ = way;
	setDp(dp_);
}
void SevenSegLed::setVisible(bool newValue)
{
	visible_ = newValue;
	if(visible_)
	{
		led_c_ = led_charbuf_c_ | (led_c_ & 0x0002);
	}
	else
	{
		led_c_ = pgm_read_word(&(LED_CHARS[0])) | (led_c_ & 0x0002);
	}
}
void SevenSegLed::setFlashOffTicks(unsigned char index, unsigned char offticks)
{
	flashOnOffTicks_c_[index] = compressFourBit(flashOnOffTicks_c_[index],offticks,0);
}
unsigned char SevenSegLed::getFlashOffTicks(unsigned char index)
{
	return uncompressFourBit(flashOnOffTicks_c_[index],0);
}
void SevenSegLed::setFlashOnTicks(unsigned char index, unsigned char onticks)
{
	flashOnOffTicks_c_[index] = compressFourBit(flashOnOffTicks_c_[index],onticks,1);
}
unsigned char SevenSegLed::getFlashOnticks(unsigned char index)
{
	return uncompressFourBit(flashOnOffTicks_c_[index],1);
}
void SevenSegLed::flash(bool dp, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn)
{
	flashCnt_[(unsigned char)dp] = flashes;
	flashTick_[(unsigned char)dp] = 0;
	setFlashOnTicks((unsigned char)dp,onTicks);
	setFlashOffTicks((unsigned char)dp,offTicks);
	if (dp)
	{
		setDp(startOn);
	}
	else
	{
		setVisible(startOn);
	}
}
void SevenSegLed::flashStop(bool dp)
{
	flashCnt_[(unsigned char)dp] = 0;
	if (dp)
	{
		setDp(true);
	}
	else
	{
		setVisible(true);
	}
}
void SevenSegLed::refreshFlash(unsigned char tick_inc)
{
	unsigned char j;
	bool state;
	unsigned char onticks = 0;
	unsigned char offticks = 0;
	
	for(j=0;j<2;++j)
	{
		if(flashCnt_[j]>0)
		{
			flashTick_[j] += tick_inc;
			if((j==0 && visible_==true) || (j==1 && dp_==true))
			{
				state = true;
			}
			else
			{
				state = false;
			}
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
				if(j==0)
				{
					setVisible(!state);
				}
				else
				{
					setDp(!state);
				}
				if(flashCnt_[j]<255)
				{
					flashCnt_[j]--;
				}
			}
		}
	}
}