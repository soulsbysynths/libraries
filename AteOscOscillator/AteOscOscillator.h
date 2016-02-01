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

#define OSC_USER_WAVELEN 128

class AteOscOscillator
{
//variables
public:
	
protected:
private:
	unsigned char table_ = 0;
	unsigned char bank_ = 0;
	bool userMode_ = false;
	unsigned int userWaveStartAddress_ = 0;
	Wavetable* userWavetable_ = NULL;
//functions
public:
	AteOscOscillator(unsigned int userWaveStartAddress);
	~AteOscOscillator();
	void copyWavetable(Wavetable& destWavetable);
	void setTable(unsigned char newTable);
	unsigned char getTable(){return table_;}
	void setBank(unsigned char newBank){bank_ = newBank;}
	unsigned char getBank(){return bank_;}
	void setUserMode(bool newUserMode);
	bool getUserMode(){return userMode_;}
	void setUserWavetableSample(unsigned char index,char newSample);
	char getUserWavetableSample(unsigned char index);
	unsigned int getWaveLength(){return OSC_WAVELEN;}
	unsigned char getUserWaveLength(){return OSC_USER_WAVELEN;}
	unsigned int getUserWaveStartAddress(){return userWaveStartAddress_;}
	void writeUserWave(unsigned char waveNum);
	void readUserWave(unsigned char waveNum);
protected:
private:
	AteOscOscillator() {}
	AteOscOscillator( const AteOscOscillator &c );
	AteOscOscillator& operator=( const AteOscOscillator &c );

}; //AteOscOscillator

#endif //__ATEOSCOSCILLATOR_H__
