//CV Inputl.cpp  Hardware analogue control (ie potentiometer)
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

#include "CvInput.h"

// default constructor
CvInput::CvInput(unsigned int initValue)
{
	value_ = initValue;
	valueLast_ = initValue;
} //CvInput

// default destructor
CvInput::~CvInput()
{
} //~CvInput
unsigned int CvInput::getOutput()
{
	if(gain_==0xFF)
	{
		return value_;
	}
	else
	{
		return  ((value_ >> 4) * (gain_+1)) >> 4;  //slightly weird maths to stop use of ulong. scale value down first.
	}
}
void CvInput::setGain(unsigned char newValue)
{
	if(newValue!=gain_)
	{
		gainChanged_ = true;
		gain_ = newValue;
	}
}
bool CvInput::hasChanged(unsigned char ticksPassed)
{
	bool changed = false;
	if(gainChanged_)
	{
		gainChanged_ = false;
		changed = true;
	}
	unsigned int diff = absdiff(value_,valueLast_);
	if(lockOut_==true)
	{
		if(diff>kCtrlMoveThreshold && moving_==false)
		{
			moving_ = true;
			moveTick_ = 0;
		}
		else if (moving_==true)
		{
			moveTick_ += ticksPassed;
			if(moveTick_>=kCtrlMoveTimeOut)
			{
				moving_ = false;
			}
		}
		
		if(moving_==true)
		{
			if(value_!=valueLast_)
			{
				valueLast_ = value_;
				changed = true;
			}
		}
	}
	else
	{
		if(diff>0)
		{
			changed = true;
			moving_ = true;
			valueLast_ = value_;
		}
		else
		{
			moving_ = false;
		}
	}
	return changed;
}