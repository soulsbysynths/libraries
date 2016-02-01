//OdyFilter.cpp  Odytron filter calc variables.  All processing done in Ody.cpp
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
	int r;
	int tmpFc = (int)fc_;
	tmpFc += ((int)fmAAmount_ * fmA) >> 4;
	tmpFc += ((int)fmBAmount_ * fmB) >> 3;
	if(kbrd>0)
	{
		kbrd_ = kbrd;
	}
	tmpFc += ((int)kbrdAmount_ * ((int)kbrd_ - 64)) >> 3;  //allow for neg offset too
	tmpFc = (int)constrainUChar(tmpFc);
	switch (type_)
	{
		case OFF:
		break;
		case MOZZI:
		c_ = (char)(tmpFc>>1);
		r =  (char)(q_>>1);
		break;
		case SIMPLE:
		c_ = (char)map(tmpFc,0,255,5,122); //0.5 - 122
		r = (char)map(q_,0,255,45,map(tmpFc,0,255,20,2)); //45 - 0.2
		//simpC_ = map(tmpFc,0,255,1,61); //0.25 - 61
		//simpR = map(q_,0,255,23,1); //23 - 0.09
		//simpRCScaled_ = (simpR*simpC_)>>6;
		break;
		case KARLSEN:
		c_ = ((((unsigned long)tmpFc*7)*TWOPI_SCALED) / SF);
		r = q_>>1;
		break;
	}
	rcScaled_ = (char)(((int)r*c_)>>SCALE);
}


//All processing now done in Ody.cpp