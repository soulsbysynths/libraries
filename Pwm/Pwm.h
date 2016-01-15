//Pwm.h  Pulse width modulation class
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

#ifndef __PWM_H__
#define __PWM_H__

#include <stdlib.h>
#include "Wavetable.h"

class Pwm
{
//variables
public:
protected:
private:
	unsigned char lfoAmount_ = 0;
	unsigned char envAmount_ = 0;
	unsigned char scale_ = 1;
	unsigned char waveLength_ = 128;
//functions
public:
	Pwm(unsigned char waveLength);
	~Pwm();
	void setLfoAmount(unsigned char newValue){lfoAmount_ = newValue;}
	unsigned char getLfoAmount(){return lfoAmount_;}
	void setEnvAmount(unsigned char newValue){envAmount_ = newValue;}
	unsigned char getEnvAmount(){return envAmount_;}
	void setWaveLength(unsigned char newValue);
	unsigned char getWaveLength(){return waveLength_;}
	void processWavetable(Wavetable& sourceWavetable, char envLevel, char lfoLevel);
protected:
private:
	Pwm() {}
	Pwm( const Pwm &c );
	Pwm& operator=( const Pwm &c );

}; //Pwm

#endif //__PWM_H__
