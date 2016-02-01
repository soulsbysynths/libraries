//OdyAmplifier.cpp  Odytron Calculate current amplitude.
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
