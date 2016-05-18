//BiquadFilter.cpp  Biquad Filter class
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "BiquadFilter.h"

// default constructor
BiquadFilter::BiquadFilter()
{
} //BiquadFilter

// default destructor
BiquadFilter::~BiquadFilter()
{
} //~BiquadFilter

void BiquadFilter::setType(BiquadFilter::FiltType newType)
{
	type_ = newType;
	switch (type_)
	{           //calculate var A for shelf and peak filters: A  = sqrt( 10^(dBgain/20) )
		case BiquadFilter::FILT_PEAK10:
		a_ = pow(10, 0.25);
		break;
		case BiquadFilter::FILT_PEAK30:
		a_ = pow(10, 0.75);
		break;
		case BiquadFilter::FILT_PEAK100:
		a_ = pow(10, 2.5);
		break;
		case BiquadFilter::FILT_LS10:
		a_ = pow(10, 0.25);
		break;
		case BiquadFilter::FILT_LS30:
		a_ = pow(10, 0.75);
		break;
		case BiquadFilter::FILT_HS10:
		a_ = pow(10, 0.25);
		break;
		case BiquadFilter::FILT_HS30:
		a_ = pow(10, 0.75);
		break;
	}
	//this is here for safety
	bi1_ = 0;
	bi2_ = 0;
	bo1_ = 0;
	bo2_ = 0;
}

void BiquadFilter::setGainAdj(bool newGainAdj)
{
	gainAdj_ = newGainAdj;
	//this is here for safety
	bi1_ = 0;
	bi2_ = 0;
	bo1_ = 0;
	bo2_ = 0;
}

void BiquadFilter::setEnvAmount(unsigned char newAmount)
{
	envAmount_ = newAmount;
	envAmountF_ = ((float)envAmount_ * MAX_ENV_LFO) / 255;
}

void BiquadFilter::setLfoAmount(unsigned char newAmount)
{
	lfoAmount_ = newAmount;
	lfoAmountF_ = ((float)lfoAmount_ * MAX_ENV_LFO) / 255;
}

void BiquadFilter::refresh(unsigned long sampleFreq, char lfoOutput, char envOutput)
{
	unsigned long max_fc;
	unsigned long f;

	if(type_>BiquadFilter::FILT_OFF)
	{
		max_fc = sampleFreq >> 1;
		max_fc -= sampleFreq >> 6;
		if(!absoluteFc_)
		{
			f = max_fc;
			f = f * (fcRel_ + 1) >> 8;
		}
		else
		{
			f = (unsigned long)fcAbs_;
		}
		
		if(lfoAmount_>0)
		{
			f = f * ((unsigned int)shapeExponential(lfoOutput,lfoAmountF_,MULT_ENV_LFO) + 1) >> BS_ENV_LFO;
		}
		if (envAmount_>0)
		{
			f = f * ((unsigned int)shapeExponential(envOutput,envAmountF_,MULT_ENV_LFO) + 1) >> BS_ENV_LFO;
		}
		if (f>max_fc)
		{
			f = max_fc;
		}
		if(f<20)
		{
			f = 20;
		}
	}

	BiquadCalculator(type_,(float)f,(float)q_ * MULT_Q + MIN_Q, a_,M_PI/sampleFreq);

}
void BiquadFilter::BiquadCalculator(FiltType type, float fc, float q, float a, float piOverSf)
{
	float w,alpha,sn,cs,sra,amcs,amps,sraalpha,apcs,k,k2p1;
	switch (type)
	{
		case FILT_OFF:
		break;
		case FILT_LPF:
		w = 2 * piOverSf * fc;
		alpha = sin(w) / q * 0.5;
		cs = cos(w);
		b0_ = (1 - cs) / 2;
		b1_ = 1 - cs;
		b2_ = b0_;
		a0_ = 1 + alpha;
		a1_ = -2 * cs;
		a2_ = 1 - alpha;
		break;
		case FILT_HPF:
		w = 2 * piOverSf * fc;
		alpha = sin(w) / q * 0.5;
		cs = cos(w);
		b0_ = (1 + cs) / 2;
		b1_ = -(1 + cs);
		b2_ = b0_;
		a0_ = 1 + alpha;
		a1_ = -2 * cs;
		a2_ = 1 - alpha;
		break;
		case FILT_BPF:
		w = 2 * piOverSf * fc;
		alpha = sin(w) / q * 0.5;
		sn = sin(w);
		cs = cos(w);
		b0_ =   sn/2;
		b1_ =   0;
		b2_ =  -b0_;
		a0_ =   1 + alpha;
		a1_ =  -2*cs;
		a2_ =   1 - alpha;
		break;
		case FILT_NOTCH:
		w = 2 * piOverSf * fc;
		alpha = sin(w) / q * 0.5;
		cs = cos(w);
		b0_ =   1;
		b1_ =  -2*cs;
		b2_ =   1;
		a0_ =   1 + alpha;
		a1_ =   b1_;
		a2_ =   1 - alpha;
		break;
		case FILT_PEAK10:
		case FILT_PEAK30:
		case FILT_PEAK100:
		w = 2 * piOverSf * fc;
		alpha = sin(w) / q * 0.5;
		cs = cos(w);
		b0_ =   1 + alpha*a;
		b1_ =  -2*cs;
		b2_ =   1 - alpha*a;
		a0_ =   1 + alpha/a;
		a1_ =   b1_;
		a2_ =   1 - alpha/a;
		break;
		case FILT_LS10:
		case FILT_LS30:
		w = 2 * piOverSf * fc;
		cs = cos(w);
		sra = sqrt(a);
		alpha = sin(w) / q * 0.5;
		amcs = (a-1)*cs;
		apcs = (a+1)*cs;
		sraalpha = 2*sra*alpha;
		b0_ =    a*( (a+1) - amcs + sraalpha );
		b1_ =  2*a*( (a-1) - apcs             );
		b2_ =    a*( (a+1) - amcs - sraalpha );
		a0_ =        (a+1) + amcs + sraalpha  ;
		a1_ =   -2*( (a-1) + apcs             );
		a2_ =        (a+1) + amcs - sraalpha  ;
		break;
		case FILT_HS10:
		case FILT_HS30:
		w = 2 * piOverSf * fc;
		cs = cos(w);
		sra = sqrt(a);
		alpha = sin(w) / q * 0.5;
		amcs = (a-1)*cs;
		apcs = (a+1)*cs;
		sraalpha = 2*sra*alpha;
		b0_ =    a*( (a+1) + amcs + sraalpha );
		b1_ = -2*a*( (a-1) + apcs             );
		b2_ =    a*( (a+1) + amcs - sraalpha );
		a0_ =        (a+1) - amcs + sraalpha  ;
		a1_ =    2*( (a-1) - apcs             );
		a2_ =        (a+1) - amcs - sraalpha  ;
		break;
		case FILT_BUTLPF:
		k = tan(fc * piOverSf);
		b2_ = k * k;
		b0_ = b2_;
		b1_ = 2 * b0_;
		a0_ = b0_ + (M_SQRT2 * k) + 1;
		a1_ = 2 * (b0_ - 1);
		a2_ = b0_ - (M_SQRT2 * k) + 1;
		break;
		case FILT_BUTHPF:
		k = tan(fc * piOverSf);
		k2p1 = k * k + 1;
		b0_ = 1;
		b2_ = b0_;
		b1_ = -2;
		a0_ = k2p1 + (M_SQRT2 * k);
		a1_ = 2 * (k2p1 - 2);
		a2_ = k2p1 - (M_SQRT2 * k);
		break;
		case FILT_BESLPF:
		w = tan(piOverSf * fc);
		b2_ = 3 * w * w;
		b0_ = b2_;
		b1_ = 2 * b0_;
		a0_ = 1 + 3 * w + b0_;
		a1_ = -2 + b1_;
		a2_ = 1 - 3 * w + b0_;
		break;
		case FILT_BESHPF:
		w = tan(piOverSf * fc);
		float w2 = w * w;
		b2_ = 3;
		b0_ = b2_;
		b1_ = -6;
		a0_ = w2 + 3 * w + 3;
		a1_ = 2 * w2 - 6;
		a2_ = w2 - 3 * w + 3;
		break;
	}
	b0_ /= a0_;
	b1_ /= a0_;
	b2_ /= a0_;
	a1_ /= a0_;
	a2_ /= a0_;
}
//Process the wavetable
void BiquadFilter::processWavetable(Wavetable& sourceWavetable)
{
	
	unsigned char i;
	char out, maxout;
	float in, fout, multb, bout;
	
	if (type_>0)
	{
		maxout = 0;
		for(i=0;i<sourceWavetable.getWaveLength();++i)
		{
			if (gainAdj_==true)
			{
				bout = BiquadProcess((float)(sourceWavetable.getSample(i) >> 2));
			}
			else
			{
				bout = BiquadProcess((float)sourceWavetable.getSample(i));
			}
			out = constrainChar((int)bout);
			sourceWavetable.setSample(i,out);
			if (gainAdj_==true)
			{
				out = abs(out);
				if (out > maxout)
				{
					maxout = out;
				}
			}
		}
		if (gainAdj_==true)
		{
			multb = 128 / (float)maxout;
			for(i=0;i<sourceWavetable.getWaveLength();++i)
			{
				in = (float)sourceWavetable.getSample(i);
				fout = in * multb;
				out = constrainChar((int)fout);
				sourceWavetable.setSample(i,out);
			}
		}
		//if(sourceWavetable.isAllSamplesSame()==true)
		//{
		//bi1_ = 0;
		//bi2_ = 0;
		//bo1_ = 0;
		//bo2_ = 0;
		//}
	}
}

float BiquadFilter::BiquadProcess(float bi0)
{
	float bo0;
	bo0 = b0_ * bi0 + b1_ * bi1_ + b2_ * bi2_ - a1_ * bo1_ - a2_ * bo2_;
	bi2_ = bi1_;
	bi1_ = bi0;
	bo2_ = bo1_;
	bo1_ = bo0;
	return bo0;
}


