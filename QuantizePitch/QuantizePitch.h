/* 
* QuantizePitch.h
*
* Created: 13/01/2016 11:05:14
* Author: Paul Soulsby
*/


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
