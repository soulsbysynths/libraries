//OdyPitch.h  Calculate current pitch
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




#ifndef __ODYPITCH_H__
#define __ODYPITCH_H__

#include "SsHelpers.h"

class OdyPitch
{
//variables
public:
	enum PitchFmASource : unsigned char
	{
		SINE,
		SQUARE
	};
	enum PitchFmBSource : unsigned char
	{
		S_AND_H,
		ADSR
	};
protected:
private:
	const float RANGE_MAX[3] = {0.693147180559945,1.386294361119891,2.0794415416798359282516963643745};
	const unsigned char RANGE_MULT[3] = {128,64,32};
	const unsigned char RANGE_BS[3] = {7,6,5};
	unsigned int input_ = 1760;
	unsigned int output_ = 1760;
	unsigned char fmAAmount_ = 0;
	unsigned char fmBAmount_ = 0;
	float fmAAmountF_ = 0;
	float fmBAmountF_ = 0;
	const unsigned char FM_A_RANGE = 0;
	unsigned char fmBRange_ = 0;
	unsigned char offset_ = 128;  //128 = 0 offset
	PitchFmASource fmASource_ = SINE;
	PitchFmBSource fmBSource_ = S_AND_H;
//functions
public:
	OdyPitch();
	~OdyPitch();
	void setInput(unsigned int newInp){input_ = newInp;}
	unsigned int getOutput(){return output_;}
	void setFmAAmount(unsigned char newAmt);
	unsigned char getFmAAmount(){return fmAAmount_;}
	void setFmBAmount(unsigned char newAmt);
	unsigned char getFmBAmount(){return fmBAmount_;}
	void setOffset(unsigned char newOffset){offset_ = newOffset;}
	unsigned char getOffset(){return offset_;}
	void setFmASource(PitchFmASource source);
	PitchFmASource getFmASource(){return fmASource_;}
	void setFmBSource(PitchFmBSource source);
	PitchFmBSource getFmBSource(){return fmBSource_;}
	void refresh(char pbend, char fmA, char fmB);
protected:
private:
	OdyPitch( const OdyPitch &c );
	OdyPitch& operator=( const OdyPitch &c );
	void calcFmBAmountF();
}; //OdyPitch

#endif //__ODYPITCH_H__
