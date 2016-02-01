//OdyOscillator.cpp  Saw and square oscillator with pwm
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


#include "OdyOscillator.h"

// default constructor
OdyOscillator::OdyOscillator()
{
} //OdyOscillator

// default destructor
OdyOscillator::~OdyOscillator()
{
} //~OdyOscillator

char OdyOscillator::getOutput(unsigned char index)
{
	if(waveform_==SAW)
	{
		return pgm_read_byte(&(SAW_WAVE[level_][index]));
	}
	else
	{
		if(index > pulseIndex_) 
		{
			return -(char)pgm_read_byte(&(SQUARE_WAVE[level_][index-pulseIndex_]));
			//return pgm_read_byte(&(SQUARE_MIN[level_]));
		} 
		else
		{
			return (char)pgm_read_byte(&(SQUARE_WAVE[level_][index]));
			//return pgm_read_byte(&(SQUARE_MAX[level_]));
		}
	}	
}

void OdyOscillator::refresh(char envOutput, char lfoOutput)
{
	int curIndex, offset;
	int modsrc;
	
	curIndex = pulseWidth_;
	if(pwmAmount_>0)
	{
		if(pwmSource_==LFO)
		{
			modsrc = (int)lfoOutput;
		}
		else
		{
			modsrc = (int)envOutput;
		}
		//modsrc >>= 2;		
		offset = (modsrc * pwmAmount_) >> 8;
		curIndex += offset;
	}
	if(curIndex>PWM_MAX)
	{
		pulseIndex_ = PWM_MAX;
	}
	else if (curIndex < PWM_MIN)
	{
		pulseIndex_ = PWM_MIN;
	}
	else
	{
		pulseIndex_ = (unsigned char)curIndex;
	}	
}