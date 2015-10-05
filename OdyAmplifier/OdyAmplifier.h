/* 
* OdyAmplifier.h
*
* Created: 04/10/2015 13:03:38
* Author: Paul Soulsby
*/


#ifndef __ODYAMPLIFIER_H__
#define __ODYAMPLIFIER_H__

#include "SsHelpers.h"

class OdyAmplifier
{
//variables
public:
	enum AmpAmSource : unsigned char
	{
		AR,
		ADSR
	};
protected:
private:
	unsigned char amp_ = 0;
	unsigned char level_ = 0;  //0-127 if you ever want to implement this
	unsigned char clip_ = 1;
	unsigned char bs_ = 6;
	unsigned char amAmount_ = 16;  //0-15 if you ever want to implement this
	AmpAmSource amSource_ = AR;
//functions
public:
	OdyAmplifier();
	~OdyAmplifier();
	void refresh(char am);
	int processSample(int sample);
	void setLevel(unsigned char newLevel){level_ = newLevel;}
	unsigned char getLevel(){return level_;}
	void setClip(unsigned char newCip);
	unsigned char getClip(){return clip_;}
	void setAmSource(AmpAmSource newSource){amSource_ = newSource;}
	AmpAmSource getAmSource(){return amSource_;}
protected:
private:
	OdyAmplifier( const OdyAmplifier &c );
	OdyAmplifier& operator=( const OdyAmplifier &c );
}; //OdyAmplifier

#endif //__ODYAMPLIFIER_H__
