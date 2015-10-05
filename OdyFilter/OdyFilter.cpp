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
	double dsimpR, dsimpC;
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
			calcFc_ = 127;
		}
		else{
			calcFc_ = (char)lcut;
		}
		calcQ_ = q_ >> 1;
		break;
		case SIMPLE:
		dsimpC = 128.0*pow(0.5, (128.0-((double)tmpFc*0.5)) / 16.0); //0.5 - 122
		dsimpR = 128.0*pow(0.5, (((double)q_*0.5)+24) / 16.0); //45 - 0.2
		simpC = (int)dsimpC;
		simpRCScaled = ((int)dsimpR*simpC)>>SCALE;		
		//calcFc_ = tmpFc >> 1;
		//calcQ_ = ((255-q_) / 6) + ((255-tmpFc) / 12) + 6;
		break;
		case MOZZI:
		mozziF = tmpFc>>1;
		mozziFb =  (q_>>1) + ((int)(q_>>1) * (127 - mozziF))>>SCALE;
		mozziFFbScaled = (mozziFb * tmpFc)>>SCALE;
		//calcFc_ = tmpFc>>1;
		//icalcQ_ = ((int)q_*3)>>2;
		break;
	}
}
int OdyFilter::processSample(int sample)
{
	int in;
	switch (type_)
	{
		case KARLSEN:
		int resoclip;
		if (buf1_ > 127){
			resoclip = 127;
		}
		else{
			resoclip = buf1_;
		}
		in = sample - iMultic(resoclip,calcQ_);
		buf0_ += iMultic(in,calcFc_) - iMultic(buf0_,calcFc_);
		buf1_ += iMultic(buf0_,calcFc_) - iMultic(buf1_,calcFc_);
		return buf1_;
		break;
		case SIMPLE:
		buf0_ -= (buf0_*simpRCScaled - buf1_*simpC + sample*simpC)>>SCALE;
		buf1_ -= (buf1_*simpRCScaled + buf0_*simpC)>>SCALE;
		//buf0_ -= iMulticc(buf0_,calcQ_,calcFc_) - iMultic(buf1_,calcFc_) + iMultic(sample,calcFc_);
		//buf1_ -= iMulticc(buf1_,calcQ_,calcFc_) + iMultic(buf0_,calcFc_);
		return buf1_;
		break;
		case MOZZI:
		buf0_ += (sample*mozziF - buf0_*mozziF + mozziFFbScaled*buf0_ - mozziFFbScaled*buf1_)>>SCALE;
		buf1_ += (buf0_*mozziF - buf1_*mozziF)>>SCALE;
		return buf1_;
		break;
	}
}