//QuantizePitchProgmem.h Pitch quantizer to 15 scales + bypass. Written for Oscitron
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
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


#ifndef QUANTIZEPITCHPROGMEM_H_
#define QUANTIZEPITCHPROGMEM_H_

#include <avr/pgmspace.h>

static const unsigned char QUANT_SCALES[16][13] PROGMEM =
{
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // octave
	},
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12  //chromatic
	},
	{
		0, 2, 4, 5, 7, 9,11,12, 0, 0, 0, 0, 0  //major
	},
	{
		0, 2, 3, 5, 7, 8,11,12, 0, 0, 0, 0, 0  //harm minor
	},
	{
		0, 2, 3, 5, 7, 8,10,12, 0, 0, 0, 0, 0  //natural minor
	},
	{
		0, 2, 4, 6, 8,10,12, 0, 0, 0, 0, 0, 0  // whole tone
	},
	{
		0, 2, 3, 5, 6, 8, 9,11,12, 0, 0, 0, 0  // diminished scale
	},
	{
		0, 2, 4, 7, 9,12, 0, 0, 0, 0, 0, 0, 0  //pentatonic scale
	},
	{
		0, 3, 5, 6, 7,10,12, 0, 0, 0, 0, 0, 0  //blues hexatonic
	},
	{
		0, 2, 3, 5, 6, 9,10,12, 0, 0, 0, 0, 0  //blues heptatonic
	},
	{
		0, 4, 7,12, 0, 0, 0, 0, 0, 0, 0, 0, 0  //major arp
	},
	{
		0, 3, 7,12, 0, 0, 0, 0, 0, 0, 0, 0, 0	//minor arp
	},
	{
		0, 3, 6, 9,12, 0, 0, 0, 0, 0, 0, 0, 0  //dim arp
	},
	{
		0, 4, 8,12, 0, 0, 0, 0, 0, 0, 0, 0, 0  // aug arp
	},
	//{
	//0, 5, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0  // sus 4th
	//},
	{
		0, 7,12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  //  5th
	},
	{
		0,12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // octave
	}
};
static const char QUANT_SCALES_MAX_INDEX[16] PROGMEM = {0,12,7,7,7,6,8,5,6,7,3,3,4,3,2,1};

//static const unsigned char QUANT_SCALES[16][12] PROGMEM =
//{
	//{
		//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // octave
	//},
	//{
		//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11  //chromatic
	//},
	//{
		//0, 2, 4, 5, 7, 9,11, 0, 0, 0, 0, 0  //major
	//},
	//{
		//0, 2, 3, 5, 7, 8,11, 0, 0, 0, 0, 0  //harm minor
	//},
	//{
		//0, 2, 3, 5, 7, 8,10, 0, 0, 0, 0, 0  //natural minor
	//},
	//{
		//0, 2, 4, 6, 8,10, 0, 0, 0, 0, 0, 0  // whole tone 
	//},
	//{
		//0, 2, 3, 5, 6, 8, 9,11, 0, 0, 0, 0  // diminished scale 
	//},
	//{
		//0, 2, 4, 7, 9, 0, 0, 0, 0, 0, 0, 0  //pentatonic scale
	//},
	//{
		//0, 3, 5, 6, 7,10, 0, 0, 0, 0, 0, 0  //blues hexatonic
	//},
	//{
		//0, 2, 3, 5, 6, 9,10, 0, 0, 0, 0, 0  //blues heptatonic
	//},
	//{
		//0, 4, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0  //major arp
	//},
	//{
		//0, 3, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0	//minor arp
	//},
	//{
		//0, 3, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0  //dim arp 
	//},
	//{
		//0, 4, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0  // aug arp
	//},
	////{
		////0, 5, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0  // sus 4th
	////},
	//{
		//0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  //  5th
	//},
	//{
		//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // octave
	//}
//};
//static const char QUANT_SCALES_MAX_INDEX[16] PROGMEM = {0,11,6,6,6,5,7,4,5,6,2,2,3,2,1,0}; 


#endif /* QUANTIZEPITCHPROGMEM_H_ */