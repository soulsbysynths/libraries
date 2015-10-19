/* 
* OdyAmplifier.cpp
*
* Created: 04/10/2015 13:03:38
* Author: Paul Soulsby
*/


#include "OdyAmplifier.h"

// default constructor
OdyAmplifier::OdyAmplifier()
{
} //OdyAmplifier

// default destructor
OdyAmplifier::~OdyAmplifier()
{
} //~OdyAmplifier

void OdyAmplifier::setClip(unsigned char newClip)
{
	clip_ = newClip;
}
void OdyAmplifier::refresh(char am)
{
	int tmpAmp = 0;
	tmpAmp = ((int)am * amAmount_) >> 4;
	tmpAmp += level_;
	amp_ = constrainChar(tmpAmp);
	if(amp_<0)
	{
		amp_ = 0;
	}
}
//All processing now done in Ody.cpp
