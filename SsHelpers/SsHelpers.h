//SsHelpers.h  Useful functions for Soulsby Synths classes
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

#ifndef __SSHELPERS_H__
#define __SSHELPERS_H__

#include <avr/pgmspace.h>
#include <math.h>

static const unsigned char compressFourBitMask[2] PROGMEM = {0x0F,0xF0};
static const unsigned char compressTwoBitMask[4] PROGMEM = {0x03,0x0C,0x30,0xC0};

inline
unsigned char shapeExponential(int8_t input, float shape_amt, float multiplier)
{
	float f, g;
	f = (float)input / 127;
	g = round(exp(f * shape_amt) * multiplier - 1);
	return (unsigned char)g;
}
inline
unsigned char compressFourBit(unsigned char input, unsigned char newValue, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(compressFourBitMask[pos]));
	unsigned char shift = 4 * pos;
	return (input & ~mask) | ((newValue<<shift) & mask);
}
inline
unsigned char uncompressFourBit(unsigned char input, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(compressFourBitMask[pos]));
	unsigned char shift = 4 * pos;
	unsigned char out = input & mask;
	return out >> shift;
}
inline
unsigned char compressTwoBit(unsigned char input, unsigned char newValue, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(compressTwoBitMask[pos]));
	unsigned char shift = 2 * pos;
	return (input & ~mask) | ((newValue<<shift) & mask);
}
inline
unsigned char uncompressTwoBit(unsigned char input, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(compressTwoBitMask[pos]));
	unsigned char shift = 2 * pos;
	unsigned char out = input & mask;
	return out >> shift;
}
inline
char constrainInt(int input)
{
	if(input>127)
	{
		return 127;
	}
	else if (input<-128)
	{
		return -128;
	}
	else
	{
		return (char)input;
	}
}
#endif //__SSHELPERS_H__
