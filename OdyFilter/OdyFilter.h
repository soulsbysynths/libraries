/* 
* OdyFilter.h
*
* Created: 23/09/2015 18:30:22
* Author: Paul Soulsby
*/


#ifndef __ODYFILTER_H__
#define __ODYFILTER_H__

#include "SsHelpers.h"
#include <math.h>

class OdyFilter
{
//variables
public:
	enum FiltType : unsigned char
	{
		KARLSEN,
		SIMPLE,
		MOZZI
	};
	enum FiltFmASource : unsigned char
	{
		S_AND_H,
		SINE
	};
	enum FiltFmBSource : unsigned char
	{
		ADSR,
		AR
	};
protected:
private:
	static const unsigned int TWOPI_SCALED = 804;
	static const unsigned int SF = 31372;
	static const unsigned char SCALE = 7;
	unsigned char fc_ = 255;
	unsigned char q_;
	FiltType type_ = KARLSEN;
	unsigned char fmAAmount_ = 0;  //0-15
	unsigned char fmBAmount_ = 0;  //0-15
	unsigned char kbrdAmount_ = 0;  //0-15
	FiltFmASource fmASource_ = S_AND_H;
	FiltFmBSource fmBSource_ = ADSR;
	char calcFc_ = 25;
	char calcQ_ = 127;
	//int icalcQ_ = 127;
	int simpRCScaled = 0;
	int simpC = 0;
	char mozziF = 0;
	int mozziFb = 0;
	int mozziFFbScaled = 0;
	int buf1_,buf0_;
//functions
public:
	OdyFilter();
	~OdyFilter();
	void setFc(unsigned char newFc){fc_ = newFc;}
	unsigned char getFc(){return fc_;}
	void setQ(unsigned char newQ){q_ = newQ;}
	unsigned char getQ(){return q_;}
	void setType(FiltType newType);
	FiltType getType(){return type_;}
	void setKybrdAmount(unsigned char newAmt){kbrdAmount_ = newAmt;}
	unsigned char getKybrdAmount(){return kbrdAmount_;}
	void setFmAAmount(unsigned char newAmt){fmAAmount_ = newAmt;}
	unsigned char getFmAAmount(){return fmAAmount_;}
	void setFmBAmount(unsigned char newAmt){fmBAmount_ = newAmt;}
	unsigned char getFmBAmount(){return fmBAmount_;}
	void setFmASource(FiltFmASource sineWave){fmASource_ = sineWave;}
	FiltFmASource getFmASource(){return fmASource_;}
	void setFmBSource(FiltFmBSource arEnv){fmBSource_ = arEnv;}
	FiltFmBSource getFmBSource(){return fmBSource_;}	
	void refresh(unsigned char kbrd, char fmA, char fmB);
	int processSample(int sample);	
protected:
private:
	OdyFilter( const OdyFilter &c );
	OdyFilter& operator=( const OdyFilter &c );

}; //OdyFilter

#endif //__ODYFILTER_H__
