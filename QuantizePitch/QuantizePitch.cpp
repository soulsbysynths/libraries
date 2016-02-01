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
	unsigned int quantized = getClosest(sourcePitch);
	unsigned int out = (((unsigned long)quantized * qntAmout) >> qntAmountBs) + (((unsigned long)sourcePitch * (qntAmountMax-qntAmout)) >> qntAmountBs);
	return out;
}

unsigned int QuantizePitch::getClosest(unsigned int val) {

	char lo = 0;
	char hi = MIDI_FREQS_MAX_INDEX;
	char best_ind = lo;
	char mid;
	while (lo <= hi)
	{
		mid = lo + (hi - lo)  /2;
		if(pgm_read_word(&(MIDI_FREQS[mid])) < val)
		{
			lo = mid + 1;
		}
		
		else if(pgm_read_word(&(MIDI_FREQS[mid])) > val)
		{
			hi = mid - 1;
		}
		else
		{
			best_ind = mid;
			break;
		}
		// check if data[mid] is closer to val than data[best_ind]
		if(absdiff(pgm_read_word(&(MIDI_FREQS[mid])),val) < absdiff(pgm_read_word(&(MIDI_FREQS[best_ind])),val))
		{
			best_ind = mid;
		}
	}

	return pgm_read_word(&(MIDI_FREQS[best_ind]));
}