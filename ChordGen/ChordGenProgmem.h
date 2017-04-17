/*
* ChordGenProgmem.h
*
* Created: 14/04/2017 12:13:32
*  Author: paulsoulsby
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