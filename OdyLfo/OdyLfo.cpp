/* 
* OdyLfo.cpp
*
* Created: 29/09/2015 09:47:27
* Author: Paul Soulsby
*/


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