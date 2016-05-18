//AteOscOscillator.h  Atmegatron Oscillator class
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

#ifndef __ATEOSCOSCILLATOR_H__
#define __ATEOSCOSCILLATOR_H__

#include <stdlib.h>
#include "Wavetable.h"
#include "AteOscOscillatorProgmem.h"

class AteOscOscillator
{
//variables
public:
	
protected:
private:
	unsigned char table_ = 0;
	unsigned char bank_ = 0;
	Wavetable* wavetable_ = NULL;
//functions
public:
	AteOscOscillator();
	~AteOscOscillator();
	void copyWavetable(Wavetable& destWavetable);
	void setTable(unsigned char newTable);
	unsigned char getTable(){return table_;}
	void setBank(unsigned char newBank);
	unsigned char getBank(){return bank_;}
	void setWavetableSample(unsigned char index,char newSample);
	char getWavetableSample(unsigned char index);
	unsigned int getWaveLength(){return OSC_WAVELEN;}
	void writeWavetable();
	void readWavetable();
	void resetWavetable();
	void resetFactory();
protected:
private:
	AteOscOscillator( const AteOscOscillator &c );
	AteOscOscillator& operator=( const AteOscOscillator &c );

}; //AteOscOscillator

#endif //__ATEOSCOSCILLATOR_H__
