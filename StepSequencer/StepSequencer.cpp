//StepSequencer.cpp  StepSequencer class
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

#include "StepSequencer.h"

// default constructor
StepSequencer::StepSequencer(StepSequencerBase* base)
{
	base_ = base;
} //StepSequencer

// default destructor
StepSequencer::~StepSequencer()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~StepSequencer

void StepSequencer::setPattern(unsigned char new_pattern)
{
	pattern_ = new_pattern;
	if(pattern_==0)
	{
		base_->stepseqNoteEvent(stepLast_,REST);
		stepLast_ = REST;
	}
}

void StepSequencer::setDivision(unsigned char newDiv)
{
	division_ = newDiv;
	divMult_ = pgm_read_word(&(seqMult[division_]));
	divBs_ = pgm_read_byte(&(seqBs[division_]));
}

void StepSequencer::reset()
{
	indexLast_ = seqLength;
	stepLast_ = REST;
}
void StepSequencer::refresh(unsigned int cycleTick)
{
	if(pattern_!=0)
	{
		unsigned char index = ((unsigned long)cycleTick * divMult_) >> divBs_;
		index &= seqBitMask;
		
		if(index!=indexLast_)
		{
			unsigned char step = pgm_read_byte(&(seqPatterns[pattern_][index]));
			base_->stepseqNoteEvent(stepLast_,step);
			stepLast_ = step;
			indexLast_ = index;
		}	
	}

}

