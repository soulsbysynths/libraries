//LedRgb.cpp  Hardware RGB LED
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

#include "LedRgb.h"

// default constructor
LedRgb::LedRgb()
{
} //Ledswitch

// default destructor
LedRgb::~LedRgb()
{
} //~Ledswitch

void LedRgb::setFlashOnCol(LedRgbColour oncol)
{
	flashOnOff_c_ = compressFourBit(flashOnOff_c_,(unsigned char)oncol,1);
}
LedRgb::LedRgbColour LedRgb::getFlashOnCol()
{
	return (LedRgbColour)uncompressFourBit(flashOnOff_c_, 1);
}
void LedRgb::setFlashOffCol(LedRgbColour offcol)
{
	flashOnOff_c_ = compressFourBit(flashOnOff_c_,(unsigned char)offcol,0);
}
LedRgb::LedRgbColour LedRgb::getFlashOffCol()
{
	return (LedRgbColour)uncompressFourBit(flashOnOff_c_,0);
}
void LedRgb::flash(unsigned char flashes, unsigned char onTicks, unsigned char offTicks, LedRgbColour on_col, LedRgbColour off_col,bool startOn)
{
	flashCnt_ = flashes;
	flashTick_ = 0;
	setFlashOnTicks(onTicks);
	setFlashOffTicks(offTicks);
	setFlashOnCol(on_col);
	setFlashOffCol(off_col);
	if(startOn==true)
	{
		setColour(on_col);
	}
	else
	{
		setColour(off_col);
	}
}
void LedRgb::flashStop()
{
	flashCnt_ = 0;
	setColour(getFlashOffCol());
}
void LedRgb::setFlashOffTicks(unsigned char offticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,offticks,0);
}
unsigned char LedRgb::getFlashOffTicks()
{
	return uncompressFourBit(flashOnOffTicks_c_,0);
}
void LedRgb::setFlashOnTicks(unsigned char onticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,onticks,1);
}
unsigned char LedRgb::getFlashOnticks()
{
	return uncompressFourBit(flashOnOffTicks_c_,1);
}
void LedRgb::refreshFlash(unsigned char tick_inc)
{
	unsigned char onticks = 0;
	unsigned char offticks = 0;
	LedRgbColour col;
	LedRgbColour col_on;
	LedRgbColour col_off;
	if(flashCnt_>0)
	{
		flashTick_ += tick_inc;
		col = getColour();
		col_on = getFlashOnCol();
		col_off = getFlashOffCol();
		onticks = getFlashOnticks();
		offticks = getFlashOffTicks();
		if((col==col_on && flashTick_>=onticks) || (col==col_off && flashTick_>=offticks))
		{
			if(col==col_on)
			{
				flashTick_ -= onticks;
				setColour(col_off);
			}
			else
			{
				flashTick_ -= offticks;
				setColour(col_on);
			}
			if(flashCnt_<255)
			{
				flashCnt_--;
			}
		}
	}	
}