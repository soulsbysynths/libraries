//AteOscPitch.h  Atmegatron Pitch class (calculate output frequency)
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

#ifndef __ATEOSCPITCH_H__
#define __ATEOSCPITCH_H__

#include "SsHelpers.h"

class AteOscPitch
{
//variables
public:
protected:
private:
	//const float MAX_ENV = 1.386294361119891;
	//const unsigned char MULT_ENV = 64; 
	//const unsigned char BS_ENV = 6;
	//const float MAX_LFO = 0.693147180559945;
	//const unsigned char MULT_LFO = 128;
	//const unsigned char BS_LFO = 7;
	const float MAX_CV = 3.4657359027997265470861606072909;
	const unsigned char MULT_CV = 8;
	const unsigned char BS_CV = 3;
	unsigned int input_;
	unsigned int output_;
//functions
public:
	AteOscPitch();
	~AteOscPitch();
	void setInput(unsigned int newInp){input_ = newInp;}
	unsigned int getOutput(){return output_;}
	void refresh(int cvOutput);
protected:
private:
	AteOscPitch( const AteOscPitch &c );
	AteOscPitch& operator=( const AteOscPitch &c );

}; //AteOscPitch

#endif //__ATEOSCPITCH_H__
