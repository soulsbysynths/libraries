//AteOscPitch.cpp  Atmegatron Pitch class (calculate output frequency)
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

#include "AteOscPitch.h"

// default constructor
AteOscPitch::AteOscPitch()
{
} //AteOscPitch

// default destructor
AteOscPitch::~AteOscPitch()
{
} //~AteOscPitch

unsigned int AteOscPitch::getOutput()
{
	unsigned int output = input_ + coarseOffset_ + fineOffset_;
	if(topHalf_==true)
	{
		output += 4096;
	}
	if(output>LIN_FREQ_MAX)  
	{
		return LIN_FREQ_MAX;
	}
	else
	{
		return output;
	}
}

static unsigned int AteOscPitch::calcFrequency(unsigned int linearFreq)
{
	unsigned int mult = linearFreq / LIN_FREQS_PER_OCT;
	unsigned int ind = linearFreq - (mult * LIN_FREQS_PER_OCT);
	unsigned long freq = (unsigned long)pgm_read_word(&(POW_TWO[mult])) * pgm_read_word(&(LIN_TO_LOG_FREQ[ind]));
	return (unsigned int)(freq >> 9);  //scale back down again. 
}