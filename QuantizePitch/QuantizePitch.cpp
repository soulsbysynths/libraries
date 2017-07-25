//QuantizePitch.cpp  Pitch quantizer to 15 scales + bypass. Written for Oscitron
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
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


#include "QuantizePitch.h"

// default constructor
QuantizePitch::QuantizePitch()
{
} //QuantizePitch

// default destructor
QuantizePitch::~QuantizePitch()
{
} //~QuantizePitch


unsigned int QuantizePitch::process(unsigned int linearFreq)
{
	unsigned int offset = LIN_FREQS_PER_OCT - ((unsigned int)qntKey * LIN_FREQS_PER_NOTE);
	unsigned int offsetPitch = linearFreq + offset;
	unsigned int pitch = offsetPitch % LIN_FREQS_PER_OCT;
	unsigned int oct = offsetPitch - pitch;
	return getClosest(pitch) + oct - offset;
}

unsigned int QuantizePitch::getClosest(unsigned int val) {

	char lo = 0;
	char hi = pgm_read_byte(&(QUANT_SCALES_MAX_INDEX[qntScale]));
	char bestIndex = lo;
	char mid;
	unsigned int test = 0;
	unsigned int best = (unsigned int)pgm_read_byte(&(QUANT_SCALES[qntScale][bestIndex])) << 6;
	while (lo <= hi)
	{
		mid = lo + (hi - lo) / 2;
		test = (unsigned int)pgm_read_byte(&(QUANT_SCALES[qntScale][mid])) << 6;
		if(test < val)
		{
			lo = mid + 1;
		}
		
		else if(test > val)
		{
			hi = mid - 1;
		}
		else
		{
			bestIndex = mid;
			best = test;
			break;
		}
		// check if data[mid] is closer to val than data[best_ind]
		if(absdiff(test,val) < absdiff(best,val))
		{
			bestIndex = mid;
			best = (unsigned int)pgm_read_byte(&(QUANT_SCALES[qntScale][bestIndex])) << 6;
		}
	}

	return best;
}
