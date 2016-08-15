/*
* QuantizePitch.cpp
*
* Created: 13/01/2016 11:05:14
* Author: Paul Soulsby
*/


#include "QuantizePitch.h"

// default constructor
QuantizePitch::QuantizePitch()
{
} //QuantizePitch

// default destructor
QuantizePitch::~QuantizePitch()
{
} //~QuantizePitch


unsigned int QuantizePitch::processPitch(unsigned int sourcePitch)
{
	unsigned int offset = LIN_FREQS_PER_OCT - ((unsigned int)qntKey * LIN_FREQS_PER_NOTE);
	unsigned int offsetPitch = sourcePitch + offset;
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
