//RotaryEncoder.cpp  Hardware rotary encoder
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

#ifndef __ROTARYENCODER_H__
#define __ROTARYENCODER_H__

#include <stdlib.h>

class RotaryEncoder
{
	//variables
	public:
	protected:
	private:
	char value_ = 0;
	int count_ = 0;
	int countLast_ = 0;
	bool updateVal_ = true;
	char maxValue_ = 16;
	bool continuous_ = true;
	bool direction_ = false;
	unsigned char debounceTicks_ = 0;
	unsigned char debounceCount_ = 0;
	//functions
	public:
	RotaryEncoder();
	~RotaryEncoder();
	void setCount(int new_count);
	int getCount(){return count_;}
	void setValue(char newValue);
	char getValue(){return value_;}
	void setDebounceTicks(unsigned char newValue){debounceTicks_ = newValue;}
	unsigned char getDebounceTicks(){return debounceTicks_;}
	void setMaxValue(char newValue);
	void setContinuous(bool newValue){continuous_ = newValue;}
	bool getContinuous(){return continuous_;}
	void setUpdateVal(bool newValue){updateVal_ = newValue;}
	bool getDirection(){return direction_;}
	bool hasChanged(unsigned char ticksPassed);
	protected:
	private:
	RotaryEncoder( const RotaryEncoder &c );
	RotaryEncoder& operator=( const RotaryEncoder &c );

}; //RotaryEncoder

#endif //__ROTARYENCODER_H__
