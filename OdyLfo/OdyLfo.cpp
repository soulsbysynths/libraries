//OdyLfo.cpp  Odytron LFO.  Free-wheeling, non-syncable (at the moment). Only outputs Ody waveforms
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


#include "OdyLfo.h"

// default constructor
OdyLfo::OdyLfo()
{
} //OdyLfo

// default destructor
OdyLfo::~OdyLfo()
{
} //~OdyLfo
void OdyLfo::setFreq(unsigned char newFreq)
{
	freq_ = newFreq;
	msPerCycle_ = 8000 / (unsigned int)freq_;  //coz freq is << 3 (i.e. *8).
	ms_ = ((unsigned long)msPerCycle_ * index_) >> 6; //reposition index to same point in cycle		
}
char OdyLfo::getOutput(LfoWave wave)
{
	if(wave!=S_AND_H){
		return (char)pgm_read_byte(&(LFO_WAVETABLE[(unsigned char)wave][index_]));
	}
	else{
		return sAndH_;
	}	
}
char OdyLfo::getExpOutput(LfoWave wave)
{
	char tmp;
	if(wave!=S_AND_H){
		tmp =  pgm_read_byte(&(LFO_WAVETABLE[(unsigned char)wave][index_]));
	}
	else{
		tmp =  sAndH_;
	}
	return convertExponentialBipolar(tmp); 
}
void OdyLfo::refresh(unsigned char ticksPassed)
{
	ms_ += ticksPassed;
	while(ms_>=msPerCycle_)
	{
		ms_ -= msPerCycle_;
	}
	unsigned char newIndex = ((unsigned long)ms_ << 6) / msPerCycle_;  //64 sample lfo (ie << 6)
	if(newIndex < index_)
	{
		sAndH_ = randomValue(-127,127);
	}
	index_ = newIndex;
}
void OdyLfo::reset()
{
	ms_ = 0;
}