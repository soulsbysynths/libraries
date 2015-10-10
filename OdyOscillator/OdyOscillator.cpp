/* 
* OdyOscillator.cpp
*
* Created: 29/09/2015 12:47:13
* Author: Paul Soulsby
*/


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
			return pgm_read_byte(&(SQUARE_WAVE_BOTTOM[level_][index-pulseIndex_]));
			//return pgm_read_byte(&(SQUARE_MIN[level_]));
		} 
		else
		{
			return pgm_read_byte(&(SQUARE_WAVE_TOP[level_][index]));
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