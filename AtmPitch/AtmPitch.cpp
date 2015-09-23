//AtmPitch.cpp  Atmegatron Pitch class (calculate output frequency)
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

#include "AtmPitch.h"

// default constructor
AtmPitch::AtmPitch()
{
} //AtmPitch

// default destructor
AtmPitch::~AtmPitch()
{
} //~AtmPitch

void AtmPitch::setEnvAmount(unsigned char newAmount)
{
	envAmount_ = newAmount;
	envAmountF_ = ((float)envAmount_ * MAX_ENV) / 255;
}

void AtmPitch::setLfoAmount(unsigned char newAmount)
{
	lfoAmount_ = newAmount;
	lfoAmountF_ = ((float)lfoAmount_ * MAX_LFO) / 255;
}

void AtmPitch::refresh(char lfoOutput, char envOutput, char pbendOutput)
{
	unsigned long f;
	f = input_;
	if(lfoAmount_>0)
	{
		f = f * ((unsigned int)shapeExponential(lfoOutput,lfoAmountF_,MULT_LFO) + 1) >> BS_LFO;
	}
	if(envAmount_>0)
	{
		f = f * ((unsigned int)shapeExponential(envOutput,envAmountF_,MULT_ENV) + 1) >> BS_ENV;
	}	
	if(pbendOutput!=0)
	{
		f = f * ((unsigned int)shapeExponential(pbendOutput,MAX_LFO,MULT_LFO) + 1) >> BS_LFO;
	}
	output_ = (unsigned int)f;
}
