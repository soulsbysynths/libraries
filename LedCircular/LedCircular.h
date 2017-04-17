//LedCircular.h  Hardware 16 LED array
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

#ifndef __LedCIRCULAR_H__
#define __LedCIRCULAR_H__

#include "SsHelpers.h"
#include <string.h>

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class LedCircular
{
//variables
public:
protected:
private:
	unsigned int led_c_ = 0;
	unsigned char flashCnt_[16] = {0};
	unsigned char flashOnOffTicks_c_[16] = {0};   //compresses on and off ticks into 4 bit
	unsigned int flashTick_[16] = {0};
//functions
public:
	LedCircular();
	~LedCircular();
	void select(unsigned char value);
	void fill(unsigned char value);
	void setState(unsigned int leds){led_c_ = leds;}
	unsigned int getState(){return led_c_;}
	void setSegment(unsigned char segment, bool led_on);
	bool getSegment(unsigned char segment);
	void flash(unsigned char segment, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn);
	void flashStop(unsigned char segment);
	void flashStopAll();
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	LedCircular( const LedCircular &c );
	LedCircular& operator=( const LedCircular &c );
	void setFlashOnTicks(unsigned char segment, unsigned char onticks);
	unsigned char getFlashOnticks(unsigned char segment);
	void setFlashOffTicks(unsigned char segment, unsigned char offticks);
	unsigned char getFlashOffTicks(unsigned char segment);
}; //LedCircular

#endif //__LedCIRCULAR_H__
