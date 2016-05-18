//BiquadFilter.h  Biquad Filter class
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation either version 3 of the License or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful

//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not see <http://www.gnu.org/licenses/>.

#ifndef __BIQUADFILTER_H__
#define __BIQUADFILTER_H__

#include <math.h>
#include <stdlib.h>
#include "Wavetable.h"
#include "SsHelpers.h"

class BiquadFilter
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
	const float MAX_ENV_LFO = 1.386294361119891;
	const unsigned char MULT_ENV_LFO = 64;
	const unsigned char BS_ENV_LFO = 6;
	const float MIN_Q = 0.5;
	const float MULT_Q = 0.076470588235294;
	unsigned char fcRel_ = 120;  //this is scaled by sample freq
	unsigned int fcAbs_ = 10000;
	unsigned char q_ = 0;
	FiltType type_ = FILT_LPF;
	bool gainAdj_ = false;
	bool absoluteFc_ = false;
	unsigned char envAmount_ = 0;
	float envAmountF_ = 0;
	unsigned char lfoAmount_ = 0;
	float lfoAmountF_ = 0;
	float a_;
	float a0_ = 1;
	float a1_;
	float a2_;
	float b0_;
	float b1_;
	float b2_;
	float bi1_;
	float bi2_;
	float bo1_;
	float bo2_;
//functions
public:
	BiquadFilter();
	~BiquadFilter();
	void setFc(unsigned char newFc){fcRel_ = newFc;}
	unsigned char getFc(){return fcRel_;}
	void setFcAbs(unsigned int newFc){fcAbs_ = newFc;}
	unsigned int getFcAbs(){return fcAbs_;}
	void setQ(unsigned char newQ){q_ = newQ;}
	unsigned char getQ(){return q_;}
	void setType(FiltType newType);
	FiltType getType(){return type_;}
	void setGainAdj(bool newGainAdj);
	bool getGainAdj(){return gainAdj_;}
	void setAbsoluteFc(bool newValue){absoluteFc_ = newValue;}
	bool getAbsoluteFc(){return absoluteFc_;}
	void setEnvAmount(unsigned char newAmount);
	unsigned char getEnvAmount(){return envAmount_;}
	void setLfoAmount(unsigned char newAmount);
	unsigned char getLfoAmount(){return lfoAmount_;}
	void refresh(unsigned long sampleFreq,char lfoOutput,char envOutput);
	void processWavetable(Wavetable& sourceWavetable);
protected:
private:
	BiquadFilter( const BiquadFilter &c );
	BiquadFilter& operator=( const BiquadFilter &c );
	void BiquadCalculator(FiltType type,float fc,float q,float a,float piOverSf);
	float BiquadProcess(float bi0);
}; //BiquadFilter

#endif //__BIQUADFILTER_H__
