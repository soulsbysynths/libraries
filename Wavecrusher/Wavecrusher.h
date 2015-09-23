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


#ifndef __WAVECRUSHER_H__
#define __WAVECRUSHER_H__

#include "Wavetable.h"

class WaveCrusher
{
//variables
public:
protected:
private:
	unsigned char sampleShift_ = 0;
	unsigned char sampleHold_ = 1;
	unsigned char type_ = 0;
//functions
public:
	WaveCrusher();
	~WaveCrusher();
	void setType(unsigned char newType);
	unsigned char getType(){return type_;}
	void setSampleShift(unsigned char newValue){sampleShift_ = newValue;}
	unsigned char getSampleShift(){return sampleShift_;}
	void setSampleHold(unsigned char newValue){sampleHold_ = newValue;}
	unsigned char getSampleHold(){return sampleHold_;}
	void processWavetable(Wavetable& sourceWavetable);
protected:
private:
	WaveCrusher( const WaveCrusher &c );
	WaveCrusher& operator=( const WaveCrusher &c );

}; //WaveCrusher

#endif //__WAVECRUSHER_H__
