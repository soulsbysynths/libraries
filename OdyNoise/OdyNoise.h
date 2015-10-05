/* 
* OdyNoise.h
*
* Created: 01/10/2015 15:21:54
* Author: Paul Soulsby
*/


#ifndef __ODYNOISE_H__
#define __ODYNOISE_H__

#include "OdyNoiseProgmem.h"
#include "SsHelpers.h"

class OdyNoise
{
//variables
public:
protected:
private:
	unsigned char level_ = 0;  //0-7 for ody
	bool pink_ = false;
	char wavetable_[256];
	unsigned char readIndex_ = 0;
	unsigned char readIndexInc_ = 0;
	unsigned char writeIndex_ = 0;
//functions
public:
	OdyNoise();
	~OdyNoise();
	void setLevel(unsigned char newLevel){level_ = newLevel;}
	unsigned char getLevel(){return level_;}
	void setPink(bool newPink){pink_ = newPink;}
	bool getPink(){return pink_;}
	void refresh();
	char getOutput();
protected:
private:
	OdyNoise( const OdyNoise &c );
	OdyNoise& operator=( const OdyNoise &c );
	char filterPink(char in);
}; //OdyNoise

#endif //__ODYNOISE_H__
