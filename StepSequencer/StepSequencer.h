//StepSequencer.h  PStepSequencer class
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

#ifndef __SS_STEPSEQUENCER_H__
#define __SS_STEPSEQUENCER_H__

#include <stdlib.h>
#include <string.h>
#include "StepSequencerProgmem.h"
#include "StepSequencerBase.h"

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


class StepSequencer
{
//variables
public:
	static const unsigned char REST = 128;
	static const unsigned char HOLD = 64;
protected:
private:
	StepSequencerBase* base_;
	unsigned char pattern_ = 0;
	unsigned char division_ = 2;
	unsigned int divMult_ = 1;
	unsigned char divBs_ = 8;
	unsigned char stepLast_ = 0;
	unsigned char step_ = 15;
	unsigned char noteLast_ = REST;
//functions
public:
	StepSequencer(StepSequencerBase* base);
	~StepSequencer();
	void setPattern(unsigned char new_pattern);
	unsigned char getPattern(){return pattern_;}
	unsigned char getStep(){return step_;}
	void setDivision(unsigned char newDiv);
	unsigned char getDivision(){return division_;}
	void refresh(unsigned int cycleTick);
	void reset();
protected:
private:
	StepSequencer() {}
	StepSequencer( const StepSequencer &c );
	StepSequencer& operator=( const StepSequencer &c );
}; //StepSequencer

#endif //__SS_STEPSEQUENCER_H__
