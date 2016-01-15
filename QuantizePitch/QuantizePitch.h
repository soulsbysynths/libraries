/* 
* QuantizePitch.h
*
* Created: 13/01/2016 11:05:14
* Author: Paul Soulsby
*/


#ifndef __QUANTIZEPITCH_H__
#define __QUANTIZEPITCH_H__

#include "QuantizePitchProgmem.h"
#define min(a,b) ((a)<(b)?(a):(b))
#define absdiff(a,b) ((a)>(b)?(a-b):(b-a))

class QuantizePitch
{
//variables
public:
protected:
private:
	const unsigned char qntAmountMax = 16;
	const unsigned char qntAmountBs = 4;
	unsigned char qntAmout = 0;
//functions
public:
	QuantizePitch();
	~QuantizePitch();
	unsigned int processPitch(unsigned int sourcePitch);
	void setQntAmount(unsigned char newValue){qntAmout = newValue;}
	unsigned char getQntAmount(){return qntAmout;}
protected:
private:
	QuantizePitch( const QuantizePitch &c );
	QuantizePitch& operator=( const QuantizePitch &c );
	unsigned int getClosest(unsigned int x);
}; //QuantizePitch

#endif //__QUANTIZEPITCH_H__
