//OdyPitch.cpp  Calculate current pitch
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



#include "OdyPitch.h"

// default constructor
OdyPitch::OdyPitch()
{
} //OdyPitch

// default destructor
OdyPitch::~OdyPitch()
{
} //~OdyPitch

void OdyPitch::refresh(char pbend, char fmA, char fmB)
{
	
	unsigned long f;
	f = input_;
	if(fmAAmount_>0)
	{
		f = f * ((unsigned int)shapeExponential(fmA,fmAAmountF_,RANGE_MULT[FM_A_RANGE]) + 1) >> RANGE_BS[FM_A_RANGE];
	}

	if(fmBAmount_>0)
	{
		f = f * ((unsigned int)shapeExponential(fmB,fmBAmountF_,RANGE_MULT[fmBRange_]) + 1) >> RANGE_BS[fmBRange_];
	}
	if(pbend!=0)
	{
		f = f * ((unsigned int)shapeExponential(pbend,RANGE_MAX[FM_A_RANGE],RANGE_MULT[FM_A_RANGE]) + 1) >> RANGE_BS[FM_A_RANGE];
	}
	if(offset_!=128)
	{
		f = f * offset_ >> 7;  //offset is pre-shaped (128 = x0)
	}

	if(f>16383)  //4095 << 2  cap to stop seizing up
	{
		output_ = 16383;
	}
	else
	{
		output_ = (unsigned int)f;	
	}
}
void OdyPitch::setFmAAmount(unsigned char newAmt)
{
	fmAAmount_ = newAmt;
	fmAAmountF_ = ((float)fmAAmount_ * RANGE_MAX[FM_A_RANGE]) / 15;
}
void OdyPitch::setFmBAmount(unsigned char newAmt)
{
	fmBAmount_ = newAmt;
	calcFmBAmountF();
}
void OdyPitch::setFmASource(PitchFmASource source)
{
	fmASource_ = source;
}
void OdyPitch::setFmBSource(PitchFmBSource source)
{
	fmBSource_ = source;
	if(source==S_AND_H)
	{
		fmBRange_ = 1;
	}
	else
	{
		fmBRange_ = 2;
	}
	calcFmBAmountF();
}
void OdyPitch::calcFmBAmountF()
{
	fmBAmountF_ = ((float)fmBAmount_ * RANGE_MAX[fmBRange_]) / 15;
}