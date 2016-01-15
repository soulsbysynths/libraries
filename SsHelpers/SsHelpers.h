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
//static const char EXP_CONVERT[128] PROGMEM = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13,13,14,15,15,16,16,17,18,19,19,20,21,22,23,24,25,26,27,28,29,30,31,32,34,35,36,38,39,41,43,44,46,48,50,52,54,56,58,61,63,65,68,71,73,76,79,82,86,89,93,96,100,104,108,112,117,121,127};
static const char EXP_CONVERT_BIPOLAR[256] PROGMEM = {-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-123,-123,-123,-123,-123,-123,-123,-123,-123,-122,-122,-122,-122,-122,-122,-122,-122,-121,-121,-121,-121,-121,-121,-120,-120,-120,-120,-120,-120,-119,-119,-119,-119,-119,-118,-118,-118,-118,-118,-117,-117,-117,-117,-116,-116,-116,-116,-115,-115,-115,-114,-114,-114,-114,-113,-113,-113,-112,-112,-112,-111,-111,-110,-110,-110,-109,-109,-108,-108,-107,-107,-107,-106,-106,-105,-105,-104,-104,-103,-103,-102,-101,-101,-100,-100,-99,-98,-98,-97,-96,-96,-95,-94,-93,-93,-92,-91,-90,-89,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-73,-72,-71,-70,-69,-67,-66,-65,-63,-62,-60,-59,-57,-56,-54,-52,-51,-49,-47,-46,-44,-42,-40,-38,-36,-34,-32,-30,-28,-26,-23,-21,-19,-16,-14,-11,-9,-6,-4,-1,2,5,8,11,14,17,20,23,27,30,34,37,41,44,48,52,56,60,64,68,73,77,82,86,91,96,101,106,111,116,122,127};

static const unsigned char COMPRESS_FOUR_BIT_MASK[2] PROGMEM = {0x0F,0xF0};
static const unsigned char COMPRESS_TWO_BIT_MASK[4] PROGMEM = {0x03,0x0C,0x30,0xC0};

//static const float CV_TO_FREQ_EXP = 0.00086295;
//static const float CV_TO_FREQ_MULT = 258.6643068;
static const float CV_TO_FREQ_EXP = 0.000846127;
static const float CV_TO_FREQ_MULT = 261.6255653;

inline
unsigned int convertCvFreq(unsigned int cv)
{
	float out = CV_TO_FREQ_MULT * exp(CV_TO_FREQ_EXP * (float)cv);
	return (unsigned int)out;
}

inline
char convertExponential(char input)
{
	return (char)pgm_read_byte(&(EXP_CONVERT[(unsigned char)input]));
}
inline
char convertInvExponential(char input)
{
	return 127-(char)pgm_read_byte(&(EXP_CONVERT[127-(unsigned char)input]));
}
inline 
char convertExponentialBipolar(char input)
{
	return (char)pgm_read_byte(&(EXP_CONVERT_BIPOLAR[(int)input+128]));
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

