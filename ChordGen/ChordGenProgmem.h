/*
//ChordGenProgmem.h  Chord generator from 2 CV inputs - written for AT-ARP
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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
*/


#ifndef CHORDGENPROGMEM_H_
#define CHORDGENPROGMEM_H_

#include <avr/pgmspace.h>

static const unsigned char CG_X = 255;

static const unsigned char CG_CHORD_DATA[12][4] PROGMEM =
{
	{
		0,CG_X,CG_X,CG_X
	},
	{
		0,3,CG_X,CG_X
	},
	{
		0,4,CG_X,CG_X
	},
	{
		0,7,CG_X,CG_X
	},
	{
		0,3,7,CG_X
	},
	{
		0,4,7,CG_X
	},
	{
		0,5,7,CG_X
	},
	{
		0,4,8,CG_X
	},
	{
		0,3,6,9
	},
	{
		0,3,7,10
	},
	{
		0,4,7,10
	},
	{
		0,4,7,11
	}
};
#endif /* CHORDGENPROGMEM_H_ */