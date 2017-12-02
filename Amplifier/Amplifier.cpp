//Amplifer.cpp  Amplifer class (ie output level)
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
#include "Amplifier.h"

// default constructor
Amplifier::Amplifier()
{
	
} //Amplifier

// default destructor
Amplifier::~Amplifier()
{
} //~AtmAmplifier

void Amplifier::processWavetable(Wavetable& sourceWavetable, char envLevel, char lfoLevel)
{
	unsigned char lfoOffset = ((unsigned int)lfoAmount_+1) * (127 - lfoLevel) >> 8;   //invert lfo here
	unsigned char envOffset = (((unsigned int)envAmount_+1) * abs(envLevel)) >> 7;
	int sample;
	for(unsigned char i=0;i<sourceWavetable.getWaveLength();++i)
	{
		sample = sourceWavetable.getSample(i);
		sample = (int)(255-lfoOffset) * sample >> 8;   //and invert back again here, otherwise you'll get silence whenever lfo is off
		sample = (int)envOffset * sample >> 8;
		sourceWavetable.setSample(i,(char)sample);
	}	
}
