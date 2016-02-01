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

#ifndef __Led_H__
#define __Led_H__
#include "SsHelpers.h"

class Led
{
//variables
public:
protected:
private:
	bool led_ = false;
	unsigned char flashCnt_ = 0;
	unsigned char flashOnOffTicks_c_ = 0x11;
	unsigned char flashTick_ = 0;
//functions
public:
	Led();
	~Led();
	void setState(bool newValue){led_ = newValue;}
	bool getState(){return led_;}
	void flash(unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn);
	void flashStop();
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	Led( const Led &c );
	Led& operator=( const Led &c );
	void setFlashOnTicks(unsigned char onticks);
	unsigned char getFlashOnticks();
	void setFlashOffTicks(unsigned char offticks);
	unsigned char getFlashOffTicks();
}; //Led

#endif //__Led_H__
