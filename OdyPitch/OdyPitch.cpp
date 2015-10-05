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

void OdyPitch::refresh(char pbend, unsigned char fmA, unsigned char fmB)
{
	
	unsigned long f;
	f = input_;
	if(fmAAmount_>0)
	{
		f = f * ((unsigned int)shapeLinear(fmA,fmAAmount_) + 1) >> BS;
	}
	if(fmBAmount_>0)
	{
		f = f * ((unsigned int)shapeLinear(fmB,fmBAmount_) + 1) >> BS;
	}
	if(offset_!=128)
	{
		f = f * offset_ >> BS;  //offset is pre-shaped
	}
	//if(pbendOutput!=0)
	//{
		//f = f * ((unsigned int)shapeExponential(pbendOutput,MAX_LFO,MULT_LFO) + 1) >> BS_LFO;
	//}
	output_ = (unsigned int)f;	
}