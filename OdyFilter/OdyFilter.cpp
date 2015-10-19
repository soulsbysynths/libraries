/* 
* OdyFilter.cpp
*
* Created: 23/09/2015 18:30:22
* Author: Paul Soulsby
*/


#include "OdyFilter.h"

// default constructor
OdyFilter::OdyFilter()
{
} //OdyFilter

// default destructor
OdyFilter::~OdyFilter()
{
} //~OdyFilter

void OdyFilter::refresh(unsigned char kbrd, char fmA, char fmB)
{
	unsigned long lcut;
	int simpR;
	int tmpFc = (int)fc_;
	tmpFc += ((int)fmAAmount_ * fmA) >> 4;
	tmpFc += ((int)fmBAmount_ * fmB) >> 4;
	tmpFc += ((int)kbrdAmount_ * kbrd) >> 4;
	tmpFc = (int)constrainUChar(tmpFc);
	switch (type_)
	{
		case OFF:
		break;
		case KARLSEN:
		karlCut_ = (char)((((unsigned long)tmpFc*7)*TWOPI_SCALED) / SF);
		karlRes_ = q_>>1;
		break;
		case MOZZI:
		mozziF_ = tmpFc>>1;
		mozziFb_ =  q_>>1;
		mozziFFbScaled_ = (mozziFb_*mozziF_)>>SCALE;
		break;
		case SIMPLE:
		simpC_ = map(tmpFc,0,255,5,122); //0.5 - 122
		simpR = map(q_,0,255,45,map(tmpFc,0,255,20,2)); //45 - 0.2
		simpRCScaled_ = (simpR*simpC_)>>SCALE;
		//simpC_ = map(tmpFc,0,255,1,61); //0.25 - 61
		//simpR = map(q_,0,255,23,1); //23 - 0.09
		//simpRCScaled_ = (simpR*simpC_)>>6;
		break;
	}
}
int OdyFilter::getCscaled()
{
	switch (type_)
	{
		case OFF:
		return 0;
		case KARLSEN:
		return (int)karlCut_;
		break;
		case MOZZI:
		return (int)mozziF_;
		break;
		case SIMPLE:
		return simpC_;
		break;
	}	
}
int OdyFilter::getRscaled()
{
	switch (type_)
	{
		case OFF:
		return 0;
		case KARLSEN:
		return (int)karlRes_;
		break;
		case MOZZI:
		return mozziFb_;
		break;
		case SIMPLE:
		return 0;
		break;
	}	
}
int OdyFilter::	getRCscaled()
{
	switch (type_)
	{
		case  OFF:
		return 0;
		case KARLSEN:
		return 0;
		break;
		case MOZZI:
		return mozziFFbScaled_;
		break;
		case SIMPLE:
		return simpRCScaled_;
		break;
	}	
}

//All processing now done in Ody.cpp