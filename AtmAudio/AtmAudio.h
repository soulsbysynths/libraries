//AtmAudio.h  Atmegatron Audio output (328P PWM pin)
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

#ifndef __ATMAUDIO_H__
#define __ATMAUDIO_H__

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Wavetable.h"

#define UPDATE_ON_ZERO 0

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class AtmAudio
{
//variables
public:
protected:
private:
	unsigned long sampleFreq_ = 14080;
//functions
public:
	AtmAudio(unsigned char waveLen);
	~AtmAudio();
	void setSampleFreq(unsigned long newSf);
	unsigned long getSampleFreq(){return sampleFreq_;}
	void pasteWavetable(Wavetable& sourceWavetable);
	void initialize();
protected:
private:
	AtmAudio() {}
	AtmAudio( const AtmAudio &c );
	AtmAudio& operator=( const AtmAudio &c );

}; //AtmAudio

#endif //__ATMAUDIO_H__
