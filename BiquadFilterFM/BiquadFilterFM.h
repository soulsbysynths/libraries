///*
//* BiquadFilterFM.h
//*
//* Created: 04/05/2016 11:39:34
//* Author: paulsoulsby
//*/
//
//
//#ifndef __BIQUADFILTERFM_H__
//#define __BIQUADFILTERFM_H__
//
//#include "Wavetable.h"
//#include "SsHelpers.h"
//#include "BiquadFilterFMBase.h"
//
//class BiquadFilterFM
//{
	////variables
	//public:
	//enum FiltType : unsigned char
	//{
		//FILT_OFF,
		//FILT_LPF,
		//FILT_HPF,
		//FILT_BPF,
		//FILT_NOTCH,
		//FILT_PEAK10,
		//FILT_PEAK30,
		//FILT_PEAK100,
		//FILT_LS10,
		//FILT_LS30,
		//FILT_HS10,
		//FILT_HS30,
		//FILT_BUTLPF,
		//FILT_BUTHPF,
		//FILT_BESLPF,
		//FILT_BESHPF
	//};
	//protected:
	//private:
	//const float  MIN_Q = 0.5;
	//const float MULT_Q = 0.076470588235294;
//
	//unsigned int fc_ = 10000;
	//unsigned char q_ = 0;
	//FiltType type_ = FILT_OFF;
	//bool gainAdj_ = false;
	//float a_;
	//float sra_;
	//float a0_ = 1;
	//float a1_;
	//float a2_;
	//float b0_;
	//float b1_;
	//float b2_;
	//float bi1_;
	//float bi2_;
	//float bo1_;
	//float bo2_;
	//BiquadFilterFMBase* base_ = NULL;
	////functions
	//public:
	//BiquadFilterFM(BiquadFilterFMBase* base);
	//BiquadFilterFM(){}
	//~BiquadFilterFM();
	//void setFc(unsigned int newFc){fc_ = newFc;}
	//unsigned int getFc(){return fc_;}
	//void setQ(unsigned char newQ){q_ = newQ;}
	//unsigned char getQ(){return q_;}
	//void setType(FiltType newType);
	//FiltType getType(){return type_;}
	//void setGainAdj(bool newGainAdj);
	//bool getGainAdj(){return gainAdj_;}
	//void refresh(unsigned long sf);
	//void processWavetable(Wavetable& sourceWavetable, unsigned int freq);
	//protected:
	//private:
	//BiquadFilterFM( const BiquadFilterFM &c );
	//BiquadFilterFM& operator=( const BiquadFilterFM &c );
	//void BiquadCalculator(float fc,float q,float sf);
	//char BiquadProcess(float bi0);
//}; //BiquadFilterFM
//
//#endif //__BIQUADFILTERFM_H__



/*
* BiquadFilterFM.h
*
* Created: 04/05/2016 11:39:34
* Author: paulsoulsby
*/


#ifndef __BIQUADFILTERFM_H__
#define __BIQUADFILTERFM_H__

#include "Wavetable.h"
#include "FixedMath.h"
#include "SsHelpers.h"
#include "BiquadFilterFMBase.h"

class BiquadFilterFM
{
	//variables
	public:
	enum FiltType : unsigned char
	{
		FILT_OFF,
		FILT_LPF,
		FILT_HPF,
		FILT_BPF,
		FILT_NOTCH,
		FILT_PEAK10,
		FILT_PEAK30,
		FILT_PEAK100,
		FILT_LS10,
		FILT_LS30,
		FILT_HS10,
		FILT_HS30,
		FILT_BUTLPF,
		FILT_BUTHPF,
		FILT_BESLPF,
		FILT_BESHPF
	};
	protected:
	private:
	static const unsigned char SCALE_BS = 12;//7;
	static const int SCALE = 4096;//128;
	static const int SCALE_X2 = SCALE * 2;
	static const int SCALE_X3 = SCALE * 3;
	static const int SCALE_X6 = SCALE * 6;
	//static const unsigned char SCALE_BS = 14;//7;
	//static const long SCALE = 16384;//128;
	//static const long SCALE_X2 = SCALE * 2;
	//static const long SCALE_X3 = SCALE * 3;
	//static const long SCALE_X6 = SCALE * 6;
	const unsigned int  MIN_Q = SCALE >> 1;
	const unsigned long  MULT_Q = ((20UL * SCALE) - MIN_Q) / 255;
	const long SQRT2 = (long)(M_SQRT2 * (float)SCALE);

	unsigned int fc_ = 10000;
	unsigned char q_ = 0;
	FiltType type_ = FILT_OFF;
	bool gainAdj_ = false;
	long a_;
	long sra_;
	long a0_ = 1;
	long a1_;
	long a2_;
	long b0_;
	long b1_;
	long b2_;
	long bi1_;
	long bi2_;
	long bo1_;
	long bo2_;
	BiquadFilterFMBase* base_ = NULL;
	//functions
	public:
	BiquadFilterFM(BiquadFilterFMBase* base);
	BiquadFilterFM(){}
	~BiquadFilterFM();
	void setFc(unsigned int newFc){fc_ = newFc;}
	unsigned int getFc(){return fc_;}
	void setQ(unsigned char newQ){q_ = newQ;}
	unsigned char getQ(){return q_;}
	void setType(FiltType newType);
	FiltType getType(){return type_;}
	void setGainAdj(bool newGainAdj);
	bool getGainAdj(){return gainAdj_;}
	void refresh(unsigned long sf);
	void processWavetable(Wavetable& sourceWavetable, unsigned int freq);
	protected:
	private:
	BiquadFilterFM( const BiquadFilterFM &c );
	BiquadFilterFM& operator=( const BiquadFilterFM &c );
	void BiquadCalculator(FiltType newType, unsigned int newFc, unsigned long newQ, unsigned long newSf);
	char BiquadProcess(char bi0);
}; //BiquadFilterFM

#endif //__BIQUADFILTERFM_H__
