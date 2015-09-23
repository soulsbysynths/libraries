//Switch.cpp  Hardware switch/button with debounce
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

#include "Switch.h"

// default constructor
Switch::Switch(unsigned char initValue, unsigned int holdTicks)
{
	state_ = initValue;
	stateLast_ = initValue;
	holdEventTicks_ = holdTicks;
} //Switch

// default destructor
Switch::~Switch()
{
} //~Switch

bool Switch::hasHeld(unsigned char ticksPassed)
{
	bool held = false;
	if(state_==HIGH)
	{
		holdTime_ += ticksPassed;
		if(holdTime_>=holdEventTicks_ && holdEventFired_==false)
		{
			held = true;
			holdEventFired_ = true;
		}
		
	}	
	return held;
}
bool Switch::hasChanged(unsigned char ticksPassed)
{
	bool changed = false;

	if(ticks_<debounceTicks_)
	{
		ticks_ += ticksPassed;
	}
	if(ticks_>=debounceTicks_)
	{
		if(state_!=stateLast_)
		{
			if(state_==HIGH && stateLast_==LOW)
			{
				holdTime_ = 0;
				holdEventFired_ = false;
				changed = true;
			}
			else if(state_==LOW && stateLast_==HIGH)
			{
				changed = true;
			}
			stateLast_ = state_;
			ticks_ = 0;
		}
	}
	return changed;
}