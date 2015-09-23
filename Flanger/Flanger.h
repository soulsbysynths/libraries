//Flanger.h  Flanger class (labelled as phaser on synth)
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

#ifndef __FLANGER_H__
#define __FLANGER_H__

#include <stdlib.h>
#include "Wavetable.h"
#include "SsHelpers.h"

class Flanger
{
//variables
public:
protected:
private:
	unsigned char waveLength_;
	unsigned char lfoAmount_ = 0;
	unsigned char envAmount_ = 0;
	unsigned char scale_ = 1;
	unsigned char fbBs_ = 1;
	unsigned char wetBs_ = 1;
	unsigned char dryBs_ = 1;
	Wavetable* wavetable_ = NULL;
	bool cleared_ = false;
//functions
public:
	Flanger(unsigned char waveLength);
	~Flanger();
	void setLfoAmount(unsigned char newValue);
	unsigned char getLfoAmount(){return lfoAmount_;}
	void setEnvAmount(unsigned char newValue);
	unsigned char getEnvAmount(){return envAmount_;}
	void processWavetable(Wavetable& sourceWavetable, char envLevel, char lfoLevel);
	void clearBuffer();
protected:
private:
	Flanger() {}
	Flanger( const Flanger &c );
	Flanger& operator=( const Flanger &c );

}; //Flanger

#endif //__FLANGER_H__
