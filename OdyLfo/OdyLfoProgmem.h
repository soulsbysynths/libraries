//OdyLfoProgmem.h  Odytron LFO.  Free-wheeling, non-syncable (at the moment). Only outputs Ody waveforms - triangle (YES THIS IS CORRECT) and square waves
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


#ifndef ODYLFOPROGMEM_H_
#define ODYLFOPROGMEM_H_

#include <avr/pgmspace.h>

static const char LFO_WAVETABLE[2][64] PROGMEM =
{
	{
		0,4,8,12,16,20,24,28,32,35,39,43,47,51,55,59,63,59,55,51,47,43,39,35,32,28,24,20,16,12,8,4,0,-4,-8,-12,-16,-20,-24,-28,-32,-35,-39,-43,-47,-51,-55,-59,-63,-59,-55,-51,-47,-43,-39,-35,-32,-28,-24,-20,-16,-12,-8,-4
	},
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127
	}
};


#endif /* ODYLFOPROGMEM_H_ */