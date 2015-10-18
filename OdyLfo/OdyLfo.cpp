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
	msPerCycle_ = 8000 / freq_;  //coz freq is << 3 (i.e. *8).
}
char OdyLfo::getOutput(LfoWave wave)
{
	if(wave!=S_AND_H){
		return pgm_read_byte(&(LFO_WAVETABLE[(unsigned char)wave][index_]));
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
	if(tmp<0){
		return -pgm_read_byte(&(EXP_CONVERT[-tmp]));
	}
	else{
		return pgm_read_byte(&(EXP_CONVERT[(unsigned char)tmp]));
	}	
}
void OdyLfo::refresh(unsigned char ticksPassed)
{
	ms_ += ticksPassed;
	if(ms_>msPerCycle_)
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