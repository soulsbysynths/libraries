//LedRgb.cpp  Hardware RGB LED control
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

#ifndef __LedRGB_H__
#define __LedRGB_H__

#include "SsHelpers.h"

class LedRgb
{
//variables
public:
	enum LedRgbColour : unsigned char
	{
		OFF,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE
	};
protected:
private:
	static const unsigned char TICK_SCALE = 16;
	LedRgbColour led_ = OFF;
	unsigned char flashOnOff_c_ = 0;
	unsigned char flashCnt_ = 0;
	unsigned char flashOnOffTicks_c_ = 0;  //compresses on and off ticks into 4 bit
	unsigned char flashTick_ = 0;
//functions
public:
	LedRgb();
	~LedRgb();
	void setColour(LedRgbColour colour){led_ = colour;}
	LedRgbColour getColour(){return led_;}
	void flash(unsigned char flashes, unsigned char onTicks, unsigned char offTicks, LedRgbColour on_col, LedRgbColour off_col, bool startOn);
	void flashStop();
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	LedRgb( const LedRgb &c );
	LedRgb& operator=( const LedRgb &c );
	void setFlashOnTicks(unsigned char onticks);
	unsigned char getFlashOnticks();
	void setFlashOffTicks(unsigned char offticks);
	unsigned char getFlashOffTicks();
	void setFlashOnCol(LedRgbColour oncol);
	LedRgbColour getFlashOnCol();
	void setFlashOffCol(LedRgbColour offcol);
	LedRgbColour getFlashOffCol();
}; //LedRgb

#endif //__LedRGB_H__
