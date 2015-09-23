//RotaryEncoder.h  Hardware rotary encoder
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

#include "RotaryEncoder.h"

// default constructor
RotaryEncoder::RotaryEncoder()
{
} //RotaryEncoder

// default destructor
RotaryEncoder::~RotaryEncoder()
{
} //~RotaryEncoder
void RotaryEncoder::setValue(char newValue)
{
	if(newValue>=maxValue_)
	{
		value_ = maxValue_-1;
	}
	else if(newValue<0)
	{
		value_ = 0;
	}
	else
	{
		value_ = newValue;
	}
}
void RotaryEncoder::setMaxValue(char newValue)
{
	maxValue_ = newValue;
	if(value_>=maxValue_)
	{
		value_ = maxValue_ - 1;
	}
}
bool RotaryEncoder::hasChanged(unsigned char ticksPassed)
{
	bool changed = false;
	
	int value_change = (count_-countLast_)/4;
	if(value_change!=0)
	{
		changed = true;
		countLast_ = count_;
		if(value_change>=0)
		{
			direction_ = true;
		}
		else
		{
			direction_ = false;
		}
		if(updateVal_==true)
		{
			value_ += value_change;
			if(continuous_==true)
			{
				if(value_>=maxValue_)
				{
					value_ -= maxValue_;
				}
				else if(value_<0)
				{
					value_ += maxValue_;
				}
			}
			else
			{
				if(value_>=maxValue_)
				{
					value_ = maxValue_-1;
				}
				else if(value_<0)
				{
					value_ = 0;
				}
			}
		}
	}
	return changed;
}