//SevenSegLed.h  7 segment display, using minimum progmem and max speed.  Written for AT-ARp
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


#ifndef __SEVENSEGLED_H__
#define __SEVENSEGLED_H__

#include "SevenSegLedProgmem.h"
#include "SsHelpers.h"

class SevenSegLed
{
//variables
public:
protected:
private:
	unsigned int led_c_ = 0;
	unsigned int led_charbuf_c_ = 0;
	unsigned char flashCnt_[2] = {0};
	unsigned char flashOnOffTicks_c_[2] = {0};   //compresses on and off ticks into 4 bit
	unsigned int flashTick_[2] = {0};
	bool visible_ = true;
	bool dp_ = false;
	bool invertDp_ = false;
//functions
public:
	SevenSegLed();
	~SevenSegLed();
	void setState(unsigned int leds){led_c_ = leds;}
	unsigned int getState(){return led_c_;}
	void setVisible(bool newValue);
	bool getVisible(){return visible_;}
	void displayHex(unsigned char newValue);
	void displayTapeRoll(unsigned char frame);
	void displayChar(char character);
	void setDp(bool way);
	bool getDp(){return dp_;}
	void setInvertDp(bool way);
	void flash(bool dp, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn);
	void flashStop(bool dp);
	//void flashStopAll();
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	SevenSegLed( const SevenSegLed &c );
	SevenSegLed& operator=( const SevenSegLed &c );
	void setFlashOnTicks(unsigned char index, unsigned char onticks);
	unsigned char getFlashOnticks(unsigned char index);
	void setFlashOffTicks(unsigned char index, unsigned char offticks);
	unsigned char getFlashOffTicks(unsigned char index);
}; //SevenSegLed

#endif //__SEVENSEGLED_H__
