//ClipDistortion.cpp  Distortion class (labelled as Distortion on synth)
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

#include "ClipDistortion.h"

// default constructor
ClipDistortion::ClipDistortion()
{
} //ClipDistortion

// default destructor
ClipDistortion::~ClipDistortion()
{
} //~ClipDistortion

void ClipDistortion::processWavetable(Wavetable& sourceWavetable)
{
	int s;
	unsigned char bs = distLevel_ >> 5;
	for (unsigned char i=0;i<sourceWavetable.getWaveLength();++i)
	{
		s = (int)sourceWavetable.getSample(i);
		s = s << bs;
		if (s > 127)
		{
			sourceWavetable.setSample(i,127);
		}
		else if (s < -128)
		{
			sourceWavetable.setSample(i,-128);
		}
		else
		{
			sourceWavetable.setSample(i,(char)s);
		}
	}
}
