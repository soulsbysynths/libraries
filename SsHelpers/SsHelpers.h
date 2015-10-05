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
#include <stdlib.h>

static const char EXP_CONVERT[128] PROGMEM = {0,1,1,2,2,3,4,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,15,16,17,18,18,19,20,20,21,22,23,23,24,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,37,38,39,40,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58,59,61,62,63,64,65,66,67,68,69,70,72,73,74,75,76,77,79,80,81,82,83,85,86,87,89,90,91,92,94,95,96,98,99,100,102,103,105,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127};
static const unsigned char COMPRESS_FOUR_BIT_MASK[2] PROGMEM = {0x0F,0xF0};
static const unsigned char COMPRESS_TWO_BIT_MASK[4] PROGMEM = {0x03,0x0C,0x30,0xC0};
static const unsigned char SCALE_SHIFT = 7;

inline
char convertExponential(char input)
{
	return pgm_read_byte(&(EXP_CONVERT[input]));
}
inline
unsigned char shapeExponential(char input, float shapeAmt, unsigned char multiplier)
{
	//zero gain = multiplier
	float f, g;
	f = (float)input / 127;
	g = round(exp(f * shapeAmt) * (float)multiplier - 1);
	return (unsigned char)g;
}
inline
unsigned char shapeLinear(unsigned char input, unsigned char shapeAmt)
{
	//zero gain = 128
	unsigned int out;
	out = ((unsigned int)input * shapeAmt) >> 9;
	out += 128;
	return (unsigned char)out;
}
inline
unsigned char compressFourBit(unsigned char input, unsigned char newValue, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(COMPRESS_FOUR_BIT_MASK[pos]));
	unsigned char shift = 4 * pos;
	return (input & ~mask) | ((newValue<<shift) & mask);
}
inline
unsigned char uncompressFourBit(unsigned char input, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(COMPRESS_FOUR_BIT_MASK[pos]));
	unsigned char shift = 4 * pos;
	unsigned char out = input & mask;
	return out >> shift;
}
inline
unsigned char compressTwoBit(unsigned char input, unsigned char newValue, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(COMPRESS_TWO_BIT_MASK[pos]));
	unsigned char shift = 2 * pos;
	return (input & ~mask) | ((newValue<<shift) & mask);
}
inline
unsigned char uncompressTwoBit(unsigned char input, unsigned char pos)
{
	unsigned char mask = pgm_read_byte(&(COMPRESS_TWO_BIT_MASK[pos]));
	unsigned char shift = 2 * pos;
	unsigned char out = input & mask;
	return out >> shift;
}
inline
char constrainChar(int input)
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
inline
unsigned char constrainUChar(int input)
{
	if(input>255)
	{
		return 255;
	}
	else if (input<0)
	{
		return 0;
	}
	else
	{
		return (unsigned char)input;
	}
}
inline
char cMultcc(char a, char b)
{
	return (char)((a*b)>>SCALE_SHIFT);
}
inline
int iMultiic(int a, int b, char c)
{
	int tmp = ((a*b)>>SCALE_SHIFT);
	return ((tmp*c)>>SCALE_SHIFT);
}
inline
int iMulticc(int a, char b, char c)
{
	char tmp = ((b*c)>>SCALE_SHIFT);
	return ((a*tmp)>>SCALE_SHIFT);
}
inline
int iMultic(int a, char b)
{
	return ((a*b)>>SCALE_SHIFT);
}
inline
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
inline
char randomValue(char min, char max)
{
	return (char) (((rand()&0xFF) * ((int)max-(int)min))>>8) + (int)min;
}
#endif //__SSHELPERS_H__

