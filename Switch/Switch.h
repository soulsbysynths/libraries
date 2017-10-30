//Switch.h  Hardware switch/button with debounce
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

#ifndef __SWITCH_H__
#define __SWITCH_H__

#include <stdlib.h>
#include "SwitchBase.h"

//#define HIGH 0x1
//#define LOW  0x0

class Switch
{
//variables
public:
protected:
private:
	SwitchBase* base_;
	static const unsigned char S_HIGH = 1;
	static const unsigned char S_LOW = 0;
	unsigned int holdEventTicks_ = 2000;
	static const unsigned char dclickEventTicks_ = 100;
	unsigned char value_ = S_LOW;
	unsigned char valueActive_ = S_LOW;
	unsigned char ticks_ = 0;
	unsigned int holdTime_ = 0;
	unsigned char dclickTime_ = 0;
	unsigned char click_ = 0;
	bool holdEventFired_ = false;
	unsigned char debounceTicks_ = 20;
	unsigned char index_;
//functions
public:
	Switch() {}
	Switch(unsigned char index, unsigned char initValue, unsigned int holdTicks, SwitchBase* base);
	Switch(unsigned char initValue, unsigned int holdTicks);
	~Switch();
	void setValue(unsigned char newValue){value_ = newValue;} 
	unsigned char getValue(){return value_;} 
	void setState(unsigned char newState){value_ = newState;}  //deprocated
	unsigned char getState(){return value_;}  //deprocated
	unsigned int getHoldTime(){return holdTime_;}
	void setDebounceTicks(unsigned char newValue){debounceTicks_ = newValue;}
	bool hasChanged(unsigned char ticksPassed);  //deprocated
	bool hasHeld(unsigned char ticksPassed);  //deprocated
	void poll(unsigned char ticksPassed);
protected:
private:
	//Switch( const Switch &c );
	//Switch& operator=( const Switch &c );
}; //Switch

#endif //__SWITCH_H__
