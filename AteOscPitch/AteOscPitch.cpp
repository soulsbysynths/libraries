//AteOscPitch.cpp  Atmegatron Pitch class (calculate output frequency)
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

#include "AteOscPitch.h"

// default constructor
AteOscPitch::AteOscPitch()
{
} //AteOscPitch

// default destructor
AteOscPitch::~AteOscPitch()
{
} //~AteOscPitch
void AteOscPitch::setInput(unsigned int newInp)
{
	input_ = newInp;
	refresh();
}
void AteOscPitch::setOffset(char newOffset)
{
	offset_ = newOffset;
	refresh();
}
void AteOscPitch::refresh()
{
	//unsigned long f = input_;// ((unsigned int)shapeExponential(cvOutput,MAX_CV,MULT_CV) + 1) >> BS_CV;
	//f = f * ((unsigned int)shapeExponential(offset_,MAX_ENV,MULT_ENV) + 1) >> BS_ENV;
	float f = (((float)offset_ / 16)+1) * (float)input_;
	//long off = ((long)input_ * offset_) >>4;
	if(f>65535)
	{
		output_ = 65535;
	}
	else
	{
		output_ = (unsigned int)f;
	}
	
}
