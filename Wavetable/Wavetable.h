//Wavetable.h  Wavetable class
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

#ifndef __WAVETABLE_H__
#define __WAVETABLE_H__

#include <string.h>
//#include <SsHelpers.h>

//#define NULL 0

class Wavetable
{
//variables
public:
protected:
private:
	unsigned char waveLength_ = 128;
	char* samples_ = NULL;
//functions
public:
	Wavetable();
	Wavetable(unsigned char waveLength);
	Wavetable(const Wavetable& wt);
	~Wavetable();
	Wavetable& operator = (const Wavetable& wt);
	unsigned char getWaveLength(){return waveLength_;}
	void setSample(unsigned char index, char newSample){if(samples_!=NULL){ samples_[index] = newSample;}}
	char getSample(unsigned char index){if(samples_!=NULL){return samples_[index];}}
	void clearSamples(); 
	bool isAllSamplesSame();
protected:
private:
	//Wavetable( const Wavetable &c );
	//Wavetable& operator=( const Wavetable &c );

}; //Wavetable

#endif //__WAVETABLE_H__
