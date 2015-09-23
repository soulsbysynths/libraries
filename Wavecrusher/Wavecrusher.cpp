//Wavecrusher.cpp  Wavecrusher class
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

#include "WaveCrusher.h"

// default constructor
WaveCrusher::WaveCrusher()
{
} //WaveCrusher

// default destructor
WaveCrusher::~WaveCrusher()
{
} //~WaveCrusher

void WaveCrusher::setType(unsigned char newType)
{
	type_ = newType;
	if(type_==0)
	{
		sampleShift_ = 0;
		sampleHold_ = 1;	
	}
	else
	{
		sampleShift_ = (type_%4) + 4;
		sampleHold_ = 1 << (type_>>2);		
	}
}

void WaveCrusher::processWavetable(Wavetable& sourceWavetable)
{
	char samp;
	for (unsigned char i=0;i<sourceWavetable.getWaveLength();++i)
	{
		if (i % sampleHold_ == 0)
		{
			samp = sourceWavetable.getSample(i);
			samp = samp >> sampleShift_;
			samp = samp << sampleShift_;
		}
		sourceWavetable.setSample(i,samp);
	}	
}