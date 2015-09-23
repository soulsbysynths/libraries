//Pwm.cpp  Pulse width modulation class
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

#include "Pwm.h"

// default constructor
Pwm::Pwm(unsigned char waveLength)
{
	waveLength_ = waveLength;
	scale_ = 256 / waveLength;
} //Pwm

// default destructor
Pwm::~Pwm()
{
} //~Pwm

void Pwm::processWavetable(Wavetable& sourceWavetable, char envLevel, char lfoLevel)
{
	unsigned char i;
	unsigned char offset;
	if (lfoAmount_>0 || envAmount_>0)
	{
		unsigned char lfoOffset = ((unsigned int)lfoAmount_+1) * (lfoLevel + 127) >> 8;
		lfoOffset /= scale_;
		unsigned char envOffset = ((unsigned int)envAmount_+1) * abs(envLevel)  >> 7;
		envOffset /= scale_;
		if(lfoOffset+envOffset>waveLength_)
		{
			offset = waveLength_;
		}
		else
		{
			offset = lfoOffset + envOffset;
		}
		for(i=0;i<offset;++i)
		{
			sourceWavetable.setSample(i,0);
		}
	}	
}