//Amplifer.h  Amplifer class (ie output level)
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

#ifndef __AMPLIFIER_H__
#define __AMPLIFIER_H__

#include <stdlib.h>
#include "Wavetable.h"

class Amplifier
{
//variables
public:
protected:
private:
	unsigned char lfoAmount_ = 0;
	unsigned char envAmount_ = 255;
//functions
public:
	Amplifier();
	~Amplifier();
	void processWavetable(Wavetable& sourceWavetable, char envLevel, char lfoLevel);
	void setEnvAmount(unsigned char newAmount){envAmount_ = newAmount;}
	unsigned char getEnvAmount(){return envAmount_;}
	void setLfoAmount(unsigned char newAmount){lfoAmount_ = newAmount;}
	unsigned char getLfoAmount(){return lfoAmount_;}
protected:
private:
	Amplifier( const Amplifier &c );
	Amplifier& operator=( const Amplifier &c );

}; //Amplifier

#endif //__AMPLIFIER_H__
