/* 
* OdyLfo.h
*
* Created: 29/09/2015 09:47:27
* Author: Paul Soulsby
*/


#ifndef __ODYLFO_H__
#define __ODYLFO_H__

#include "OdyLfoProgmem.h"
#include "SsHelpers.h"

class OdyLfo
{
//variables
public:
	enum LfoWave : unsigned char
	{
		SINE,
		SQUARE,
		S_AND_H
	};
protected:
private:
	unsigned char freq_ = 80;  //this is actually freq >> 3 (ie 10Hz)
	unsigned int msPerCycle_ = 100;
	unsigned char index_ = 0;
	unsigned int ms_ = 0;  
	char sAndH_ = 0;
//functions
public:
	OdyLfo();
	~OdyLfo();
	void setFreq(unsigned char newFreq);
	unsigned char getFreq(){return freq_;}
	char getOutput(LfoWave wave);
	char getExpOutput(LfoWave wave);
	void refresh(unsigned char ticksPassed);
protected:
private:
	OdyLfo( const OdyLfo &c );
	OdyLfo& operator=( const OdyLfo &c );

}; //OdyLfo

#endif //__ODYLFO_H__
