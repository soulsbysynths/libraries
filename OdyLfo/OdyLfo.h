//OdyLfo.h  Odytron LFO.  Free-wheeling, non-syncable (at the moment). Only outputs Ody waveforms
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

#ifndef __ODYLFO_H__
#define __ODYLFO_H__

#include "OdyLfoProgmem.h"
#include "SsHelpers.h"

class OdyLfo
{
//variables
public:
	enum LfoWave : unsigned char
	{
		SINE,
		SQUARE,
		S_AND_H
	};
protected:
private:
	unsigned char freq_ = 80;  //this is actually freq >> 3 (ie 10Hz)
	unsigned int msPerCycle_ = 100;
	unsigned char index_ = 0;
	unsigned int ms_ = 0;  
	char sAndH_ = 0;
//functions
public:
	OdyLfo();
	~OdyLfo();
	void setFreq(unsigned char newFreq);
	unsigned char getFreq(){return freq_;}
	char getOutput(LfoWave wave);
	char getExpOutput(LfoWave wave);
	void refresh(unsigned char ticksPassed);
	void reset();
protected:
private:
	OdyLfo( const OdyLfo &c );
	OdyLfo& operator=( const OdyLfo &c );

}; //OdyLfo

#endif //__ODYLFO_H__
