//AtmPitch.h  Atmegatron Pitch class (calculate output frequency)
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

#ifndef __ATMPITCH_H__
#define __ATMPITCH_H__

#include "SsHelpers.h"

class AtmPitch
{
//variables
public:
protected:
private:
	const float MAX_ENV = 1.386294361119891;
	const unsigned char MULT_ENV = 64; 
	const unsigned char BS_ENV = 6;
	const float MAX_LFO = 0.693147180559945;
	const unsigned char MULT_LFO = 128;
	const unsigned char BS_LFO = 7;
	unsigned int input_;
	unsigned int output_;
	unsigned char envAmount_;
	float envAmountF_;
	unsigned char lfoAmount_;
	float lfoAmountF_;
//functions
public:
	AtmPitch();
	~AtmPitch();
	void setInput(unsigned int newInp){input_ = newInp;}
	unsigned int getOutput(){return output_;}
	void refresh(char lfoOutput, char envOutput, char pbendOutput);
	void setEnvAmount(unsigned char newAmount);
	unsigned char getEnvAmount(){return envAmount_;}
	void setLfoAmount(unsigned char newAmount);
	unsigned char getLfoAmount(){return lfoAmount_;}
	
protected:
private:
	AtmPitch( const AtmPitch &c );
	AtmPitch& operator=( const AtmPitch &c );

}; //AtmPitch

#endif //__ATMPITCH_H__
