/* 
* OdyHpf.cpp
*
* Created: 29/09/2015 09:30:45
* Author: Paul Soulsby
*/


#include "OdyHpf.h"

// default constructor
OdyHpf::OdyHpf()
{
} //OdyHpf

// default destructor
OdyHpf::~OdyHpf()
{
} //~OdyHpf

void OdyHpf::setFc(unsigned char newFc)
{
	fc_ = newFc;
	invFc_ = 255 - newFc;
}

int OdyHpf::processSample(int sample)
{
	lpf_ = ((sample * fc_) + (lpf_ * invFc_))>>8;
	return sample - lpf_;
}