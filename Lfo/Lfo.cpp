//Lfo.cpp  Low frequency oscillator class
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

#include "Lfo.h"

// default constructor
Lfo::Lfo()
{
} //Lfo

// default destructor
Lfo::~Lfo()
{
} //~Lfo
void Lfo::setDivision(unsigned char newDiv)
{
	division_ = newDiv;
	divMult_ = pgm_read_word(&(LFO_MULT[division_]));
	divBs_ = pgm_read_byte(&(LFO_BS[division_]));
}
void Lfo::refresh(unsigned int cycleTick)
{
	unsigned char index = ((unsigned long)cycleTick * divMult_) >> divBs_;
	index &= LFO_BIT_MASK;
	output_ = pgm_read_byte(&(LFO_WAVETABLE[table_][index]));
	if(invert_==true)
	{
		output_ *= -1;
	}
}