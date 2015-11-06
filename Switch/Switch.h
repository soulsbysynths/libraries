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

//#define HIGH 0x1
//#define LOW  0x0

class Switch
{
//variables
public:
protected:
private:
	static const unsigned char S_HIGH = 1;
	static const unsigned char S_LOW = 0;
	unsigned char state_ = S_LOW;
	unsigned char ticks_ = 0;
	unsigned char stateLast_ = S_LOW;
	unsigned int holdTime_ = 0;
	bool holdEventFired_ = false;
	unsigned char debounceTicks_ = 100;
	unsigned int holdEventTicks_ = 2000;
//functions
public:
	Switch() {}
	Switch(unsigned char initValue, unsigned int holdTicks);
	~Switch();
	void setState(unsigned char newState){state_ = newState;}
	unsigned char getState(){return state_;}
	unsigned int getHoldTime(){return holdTime_;}
	void setDebounceTicks(unsigned char newValue){debounceTicks_ = newValue;}
	bool hasChanged(unsigned char ticksPassed);
	bool hasHeld(unsigned char ticksPassed);
protected:
private:
	//Switch( const Switch &c );
	//Switch& operator=( const Switch &c );

}; //Switch

#endif //__SWITCH_H__
