//QuantizePitch.h  Pitch quantizer to 15 scales + bypass. Written for Oscitron
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

#ifndef __QUANTIZEPITCH_H__
#define __QUANTIZEPITCH_H__

#include "QuantizePitchProgmem.h"
#include "AteOscPitchProgmem.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define absdiff(a,b) ((a)>(b)?(a-b):(b-a))

class QuantizePitch
{
	//variables
	public:
	protected:
	private:
	unsigned char qntScale = 0;
	unsigned char qntKey = 0;  //0 = C
	//functions
	public:
	QuantizePitch();
	~QuantizePitch();
	unsigned int process(unsigned int linearFreq);
	void setQntScale(unsigned char newValue){qntScale = newValue;}
	unsigned char getQntScale(){return qntScale;}
	void setQntKey(unsigned char newValue){qntKey = newValue;}
	unsigned char getQntKey(){return qntKey;}
	protected:
	private:
	QuantizePitch( const QuantizePitch &c );
	QuantizePitch& operator=( const QuantizePitch &c );
	unsigned int getClosest(unsigned int x);
}; //QuantizePitch

#endif //__QUANTIZEPITCH_H__
