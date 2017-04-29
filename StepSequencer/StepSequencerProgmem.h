//StepSequencerProgmem.h  Progmem for StepSequencer class
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

#ifndef STEPSEQUENCER_PROGMEM_H_
#define STEPSEQUENCER_PROGMEM_H_
#define MINI 0
#define XMAS 1
#define ODY 2
#include <avr/pgmspace.h>

static const unsigned char REST = 128;
static const unsigned char HOLD = 64;
static const unsigned int SEQ_MULT[16] PROGMEM = {1,1,1,85,3,1,85,3,1,341,1,341,1,1365,1,1};
static const unsigned char SEQ_BS[16] PROGMEM = {12,11,10,16,11,9,15,10,8,16,7,15,6,16,5,4};
static const unsigned char SEQ_BIT_MASK = 15;
static const unsigned char SEQ_LENGTH = 16;

#if SEQUENCE==MINI
static const unsigned char seqPatterns[16][16] PROGMEM =
{
	
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{43,39,36,39,31,27,24,27,19,15,12,15,7,3,0,3},
	{0,60,4,57,9,52,12,48,16,45,21,40,24,36,28,45},
	{16,HOLD,31,0,31,HOLD,16,0,16,HOLD,31,0,31,HOLD,16,REST},
	{21,5,2,5,7,5,4,0,21,5,2,5,7,5,4,0},
	{2,5,2,5,4,7,4,7,0,4,0,4,9,1,9,1},
	{0,HOLD,REST,7,HOLD,REST,3,HOLD,REST,5,HOLD,REST,2,REST,2,2},
	{12,33,19,14,12,21,7,2,2,4,7,21,14,16,19,33},
	{0,REST,3,3,5,REST,7,7,8,REST,22,22,23,23,15,HOLD},
	{0,HOLD,7,REST,0,HOLD,7,4,REST,4,7,REST,0,4,REST,4},
	{0,3,7,10,12,15,19,22,24,27,31,34,36,39,43,46},
	{0,HOLD,12,HOLD,11,HOLD,7,HOLD,12,HOLD,HOLD,REST,11,HOLD,7,HOLD},
	{41,36,29,24,17,12,5,0,39,36,29,24,17,12,5,0},
	{0,HOLD,5,7,0,0,5,7,0,HOLD,5,7,0,5,7,12},
	{0,12,0,7,0,10,0,7,0,9,0,7,0,10,0,7},
	{24,23,26,21,26,19,24,18,24,16,24,14,24,12,24,11}

};
#elif SEQUENCE==ODY
static const unsigned char seqPatterns[16][16] PROGMEM =
{
	
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{
	0,7,12,19,7,12,19,24,36,31,24,19,24,19,12,7
	},
	{
	0,REST,3,1,0,REST,7,5,0,3,12,10,0,12,10,8
	},
	{
		12,24,0,HOLD,24,0,24,HOLD,12,24,0,HOLD,24,12,24,12
	},
	{
		0,7,10,12,0,5,10,12,2,7,10,12,2,5,10,12
	},
	{
		0,HOLD,REST,0,REST,REST,12,0,5,HOLD,REST,7,REST,REST,12,7
	},
	{
		36,34,31,29,31,29,27,26,24,22,19,17,12,7,3,0
	},
	{
		0,12,7,0,12,24,0,12,17,0,12,17,0,12,19,24
	},
	{
		0,REST,0,REST,0,HOLD,REST,0,3,HOLD,REST,3,HOLD,HOLD,REST,1
	},
	{
		0,12,0,12,36,24,12,24,0,12,0,12,36,24,36,12
	},
	{
		0,4,HOLD,REST,5,REST,5,7,REST,5,HOLD,REST,7,9,5,2
	},
	{
		4,7,9,7,14,12,14,16,4,7,9,7,14,12,14,16
	},
	{
		2,0,3,2,0,7,5,3,2,0,10,7,5,3,2,0
	},
	{
		19,17,19,17,12,10,12,10,7,5,7,5,2,0,2,0
	},
	{
		0,7,3,10,7,12,10,15,12,19,15,22,19,24,22,27
	},
	{
		12,HOLD,0,HOLD,12,14,0,HOLD,10,HOLD,0,HOLD,8,HOLD,0,HOLD
	}

};
#elif SEQUENCE==XMAS
static const unsigned char seqPatterns[16][16] PROGMEM =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{
		7,9,HOLD,7,4,HOLD,12,HOLD,9,HOLD,7,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,12,HOLD,11,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		5,7,HOLD,5,2,HOLD,11,HOLD,9,HOLD,7,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,9,HOLD,4,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,HOLD,7,4,HOLD,12,HOLD,9,HOLD,7,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,12,HOLD,11,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		5,7,HOLD,5,2,HOLD,11,HOLD,9,HOLD,7,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,14,HOLD,12,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		9,REST,9,HOLD,12,HOLD,9,HOLD,7,	HOLD,4,7,7,HOLD,HOLD,REST
	},
	{
		5,HOLD,9,HOLD,7,HOLD,5,HOLD,4,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		2,HOLD,4,HOLD,7,HOLD,9,HOLD,11,REST,11,REST,11,HOLD,HOLD,REST
	},
	{
		12,REST,12,REST,11,HOLD,9,HOLD,7,HOLD,5,2,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,HOLD,7,4,HOLD,12,HOLD,9,HOLD,7,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,12,HOLD,11,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	},
	{
		7,9,7,9,7,HOLD,14,HOLD,12,HOLD,HOLD,HOLD,HOLD,HOLD,HOLD,REST
	}
};
#endif


//0,0,0,4,0,0,5,0,0,7,0,0,10,0,10,12

//0,0,0,7,0,0,7,0,7,0,0,7,0,7,0,7

#endif /* STEPSEQUENCER_PROGMEM_H_ */