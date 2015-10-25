/* 
* OdyPitch.h
*
* Created: 28/09/2015 16:24:10
* Author: Paul Soulsby
*/


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
	unsigned int input_;
	unsigned int output_;
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
