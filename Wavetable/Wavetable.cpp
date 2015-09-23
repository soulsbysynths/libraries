//Wavetable.cpp  Wavetable class
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
#include "Wavetable.h"

// default constructor
Wavetable::Wavetable() : samples_(NULL), waveLength_(0)
{
} //Wavetable

Wavetable::Wavetable(unsigned char waveLength)
{
	waveLength_ = waveLength;
	samples_ = new char[waveLength_];
}

Wavetable::Wavetable(const Wavetable& wt)
{
	waveLength_ = wt.waveLength_;
	samples_ = new char[wt.waveLength_];
	for(unsigned char i=0;i<waveLength_;++i)
	{
		samples_[i] = wt.samples_[i];
	}
}
// default destructor
Wavetable::~Wavetable()
{
	if(samples_ != NULL)
	{
		delete [] samples_;
	}
} //~Wavetable

Wavetable& Wavetable::operator = (const Wavetable& wt)
{
	if (&wt == this)
	{
		return *this;
	}
	if(samples_ != NULL)
	{
		delete [] samples_;
	}
	waveLength_ = wt.waveLength_;
	samples_ = new char[wt.waveLength_];
	for(unsigned char i=0;i<waveLength_;++i)
	{
		samples_[i] = wt.samples_[i];
	}
	return *this;
}

void Wavetable::clearSamples()
{
	memset(samples_,0,sizeof(*samples_));
}

bool Wavetable::isAllSamplesSame()
{
	for(unsigned char i=1;i<waveLength_;++i)
	{
		if(samples_[i]!=samples_[0])
		{
			return false;
		}
	}
	return true;
}