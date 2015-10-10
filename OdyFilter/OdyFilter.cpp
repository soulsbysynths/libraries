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
void OdyFilter::setType(FiltType newType)
{
	type_ = newType;
	buf0_ = 0;
	buf1_ = 0;
}
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
		case KARLSEN:
		lcut = (TWOPI_SCALED * ((unsigned long)tmpFc<<4)) / SF;
		if(lcut>127){
			karlCut_ = 127;
		}
		else{
			karlCut_ = (char)lcut;
		}
		karlRes_ = q_ >> 1;
		break;
		case SIMPLE:
		simpC_ = map(tmpFc,0,255,5,122); //0.5 - 122
		//simpR = map(q_,0,255,45,3);
		//simpBS_ = map(q_,0,255,0,3);
		simpR = map(q_,0,255,45,map(tmpFc,0,255,20,2)); //45 - 0.2
		simpRCScaled_ = (simpR*simpC_)>>SCALE;		
		break;
		case MOZZI:
		mozziF_ = tmpFc>>1;
		mozziFb_ =  (q_>>1) + ((int)(q_>>1) * (127 - mozziF_))>>SCALE;
		mozziFFbScaled_ = (mozziFb_ * tmpFc)>>SCALE;
		break;
	}
}
int OdyFilter::getCscaled()
{
	switch (type_)
	{
		case KARLSEN:
		return (int)karlCut_;
		break;
		case SIMPLE:
		return simpC_;
		break;
		case MOZZI:
		return (int)mozziF_;
		break;
	}	
}
int OdyFilter::getRscaled()
{
	switch (type_)
	{
		case KARLSEN:
		return (int)karlRes_;
		break;
		case SIMPLE:
		break;
		case MOZZI:
		return mozziFb_;
		break;
	}	
}
int OdyFilter::	getRCscaled()
{
	switch (type_)
	{
		case KARLSEN:
		break;
		case SIMPLE:
		return simpRCScaled_;
		break;
		case MOZZI:
		return mozziFFbScaled_;
		break;
	}	
}
int OdyFilter::processSample(int sample)
{
	switch (type_)
	{
		case KARLSEN:
		if (buf1_ > 256)
		{
			sample -= (256*karlRes_)>>SCALE;
		}
		else
		{
			sample -= (buf1_*karlRes_)>>SCALE;
		}
		buf0_ += ((sample*karlCut_)-(buf0_*karlCut_))>>SCALE;
		buf1_ += ((buf0_*karlCut_)-(buf1_*karlCut_))>>SCALE;
		break;
		case SIMPLE:
		//sample >>= simpBS_;
		buf0_ -= (buf0_*simpRCScaled_ - buf1_*simpC_ + sample*simpC_)>>SCALE;
		buf1_ -= (buf1_*simpRCScaled_ + buf0_*simpC_)>>SCALE;
		break;
		case MOZZI:
		buf0_ += (sample*mozziF_ - buf0_*mozziF_ + mozziFFbScaled_*buf0_ - mozziFFbScaled_*buf1_)>>SCALE;
		buf1_ += (buf0_*mozziF_ - buf1_*mozziF_)>>SCALE;
		break;
	}
	return buf1_;
}