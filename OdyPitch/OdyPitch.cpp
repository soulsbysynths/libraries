/* 
* OdyPitch.cpp
*
* Created: 28/09/2015 16:24:10
* Author: Paul Soulsby
*/


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

	if(f>2047)
	{
		output_ = 2047;
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