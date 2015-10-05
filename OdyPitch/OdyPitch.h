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
	const unsigned char BS = 7;   //MULT = 128, but this is hard coded into shapeLinear
	unsigned int input_;
	unsigned int output_;
	unsigned char fmAAmount_ = 0;
	unsigned char fmBAmount_ = 0;
	unsigned char offset_ = 128;  //128 = 0 offset
	PitchFmASource fmASource_ = SINE;
	PitchFmBSource fmBSource_ = S_AND_H;
//functions
public:
	OdyPitch();
	~OdyPitch();
	void setInput(unsigned int newInp){input_ = newInp;}
	unsigned int getOutput(){return output_;}
	void setFmAAmount(unsigned char newAmt){fmAAmount_ = newAmt;}
	unsigned char getFmAAmount(){return fmAAmount_;}
	void setFmBAmount(unsigned char newAmt){fmBAmount_ = newAmt;}
	unsigned char getFmBAmount(){return fmBAmount_;}
	void setOffset(unsigned char newOffset){offset_ = newOffset;}
	unsigned char getOffset(){return offset_;}
	void setFmASource(PitchFmASource sqWave){fmASource_ = sqWave;}
	PitchFmASource getFmASource(){return fmASource_;}
	void setFmBSource(PitchFmBSource adsrEnv){fmBSource_ = adsrEnv;}
	PitchFmBSource getFmBSource(){return fmBSource_;}
	void refresh(char pbend, unsigned char fmA, unsigned char fmB);
protected:
private:
	OdyPitch( const OdyPitch &c );
	OdyPitch& operator=( const OdyPitch &c );

}; //OdyPitch

#endif //__ODYPITCH_H__
