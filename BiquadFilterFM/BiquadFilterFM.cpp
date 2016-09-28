/*
* BiquadFilterFM.cpp
*
* Created: 04/05/2016 11:39:34
* Author: paulsoulsby
*/


#include "BiquadFilterFM.h"

// default constructor
BiquadFilterFM::BiquadFilterFM(BiquadFilterFMBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //BiquadFilterFM

// default destructor
BiquadFilterFM::~BiquadFilterFM()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~BiquadFilterFM

void BiquadFilterFM::setType(FiltType newType)
{
	double a;
	type_ = newType;
	switch (type_)
	{
		case FILT_PEAK10:
		case FILT_LS10:
		case FILT_HS10:
		a = pow(10, 0.25);
		break;
		case FILT_PEAK30:
		case FILT_LS30:
		case FILT_HS30:
		a = pow(10, 0.75);  // pow(10, 0.75);
		break;
		case FILT_PEAK100:
		//da_ = Math.Pow(10, 2.5); //pow(10, 2.5);
		a = pow(10, 0.9); //pow(10, 2.5);
		break;
	}
	a_ = (long)(a * SCALE);
	sra_ = (long)(sqrt(a) * SCALE);
	//this is here for safety
	bi1_ = 0;
	bi2_ = 0;
	bo1_ = 0;
	bo2_ = 0;
}

void BiquadFilterFM::setGainAdj(bool newGainAdj)
{
	gainAdj_ = newGainAdj;
	//this is here for safety
	bi1_ = 0;
	bi2_ = 0;
	bo1_ = 0;
	bo2_ = 0;
}

void BiquadFilterFM::refresh(unsigned long sf)
{
	unsigned long max_fc;
	unsigned long min_fc;
	unsigned long fc = fc_;
	unsigned long q = q_ * MULT_Q + MIN_Q;

	if (type_ > FILT_OFF)
	{
		if (type_ >= FILT_BUTLPF)
		{
			min_fc = (unsigned long)(sf >> 7);
			max_fc = (unsigned long)((sf >> 1) - (sf >> 4));
		}
		else
		{
			min_fc = (unsigned long)(sf >> 8);
			max_fc = (unsigned long)((sf >> 1) - (sf >> 6));
		}


		if (fc > max_fc)
		{
			fc = max_fc;
		}
		if (fc < min_fc)
		{
			fc = min_fc;
		}
	}

	BiquadCalculator(fc, q, sf);

}

void BiquadFilterFM::BiquadCalculator(unsigned int fc, unsigned long q, unsigned long sf)
{
	switch (type_)
	{
		case FILT_OFF:
		break;
		case FILT_LPF:
		case FILT_HPF:
		case FILT_BPF:
		case FILT_NOTCH:
		case FILT_PEAK10:
		case FILT_PEAK30:
		case FILT_PEAK100:
		case FILT_LS10:
		case FILT_LS30:
		case FILT_HS10:
		case FILT_HS30:
		{
			unsigned int w = (unsigned int)(((unsigned long)fc << FIXEDMATH_BS) / sf);  //w = 2 * piOverSf * fc;
			int cs = fcos(w);  //cs = cos(w);
			int sn = fsin(w);
			long alpha = (((long)sn << SCALE_BS) / (long)q) >> 1;  //alpha = sin(w) / q * 0.5;
			switch (type_)
			{
				case FILT_LPF:
				{
					b0_ = (SCALE - cs) >> 1;  //b0_ = (1 - cs) / 2;
					b1_ = SCALE - cs;  //b1_ = 1 - cs;
					b2_ = b0_;  //b2_ = b0_;
					a0_ = SCALE + alpha;  //a0_ = 1 + alpha;
					a1_ = -(cs << 1);  //a1_ = -2 * cs;
					a2_ = SCALE - alpha;  //a2_ = 1 - alpha;
				}
				break;
				case FILT_HPF:
				{
					b0_ = (SCALE + cs) >> 1; //b0_ = (1 + cs) / 2;
					b1_ = -(SCALE + cs);  //b1_ = -(1 + cs);
					b2_ = b0_;
					a0_ = SCALE + alpha;  //a0_ = 1 + alpha;
					a1_ = -(cs << 1);  //a1_ = -2 * cs;
					a2_ = SCALE - alpha;  //a2_ = 1 - alpha;
				}
				break;
				case FILT_BPF:
				{
					b0_ = sn >> 1;    //b0_ =   sn/2;
					b1_ = 0;    //b1_ =   0;
					b2_ = -b0_; //b2_ =  -b0_;
					a0_ = SCALE + alpha; //a0_ =   1 + alpha;
					a1_ = -(cs << 1); //a1_ =  -2*cs;
					a2_ = SCALE - alpha;   //a2_ =   1 - alpha;
				}

				break;
				case FILT_NOTCH:
				{
					b0_ = SCALE;    //b0_ =   1;
					b1_ = -(cs << 1);   //b1_ =  -2*cs;
					b2_ = SCALE;             //b2_ =   1;
					a0_ = SCALE + alpha;     //a0_ =   1 + alpha;
					a1_ = b1_;               //a1_ =   b1_;
					a2_ = SCALE - alpha;     //a2_ =   1 - alpha;
				}
				break;
				case FILT_PEAK10:
				case FILT_PEAK30:
				case FILT_PEAK100:
				{
					b0_ = SCALE + ((alpha * a_) >> SCALE_BS); //b0_ =   1 + alpha*a;
					b1_ = -(cs << 1);    //b1_ =  -2*cs;
					b2_ = SCALE - ((alpha * a_) >> SCALE_BS);  //b2_ =   1 - alpha*a;
					a0_ = SCALE + (alpha << SCALE_BS) / a_;   //a0_ =   1 + alpha/a;
					a1_ = b1_;   //a1_ =   b1_;
					a2_ = SCALE - (alpha << SCALE_BS) / a_;   //a2_ =   1 - alpha/a;
				}
				break;
				case FILT_LS10:
				case FILT_LS30:
				case FILT_HS10:
				case FILT_HS30:
				{
					long amcs = ((a_ * cs) >> SCALE_BS) - cs; //(a-1)*cs;
					long apcs = ((a_ * cs) >> SCALE_BS) + cs; //(a+1)*cs;
					long sraalpha = (sra_ * alpha) >> (SCALE_BS - 1); //sraalpha = 2*sra*alpha;
					switch (type_)
					{
						case FILT_LS10:
						case FILT_LS30:
						{
							b0_ = (a_ * ((a_ + SCALE) - amcs + sraalpha)) >> SCALE_BS; //b0_ =    a_*( (a_+1) - amcs + sraalpha );
							b1_ = (a_ * ((a_ - SCALE) - apcs)) >> (SCALE_BS - 1); //b1_ =  2*a_*( (a_-1) - apcs             );
							b2_ = (a_ * ((a_ + SCALE) - amcs - sraalpha)) >> SCALE_BS;    //b2_ =    a_*( (a_+1) - amcs - sraalpha );
							a0_ = (a_ + SCALE) + amcs + sraalpha; //a0_ =        (a_+1) + amcs + sraalpha  ;
							a1_ = -(((a_ - SCALE) + apcs) << 1);  //a1_ =   -2*( (a_-1) + apcs             );
							a2_ = (a_ + SCALE) + amcs - sraalpha; //a2_ =        (a_+1) + amcs - sraalpha  ;
						}
						break;
						case FILT_HS10:
						case FILT_HS30:
						{
							b0_ = (a_ * ((a_ + SCALE) + amcs + sraalpha)) >> SCALE_BS;   //a_*( (a_+1) + amcs + sraalpha );
							b1_ = -(a_ * ((a_ - SCALE) + apcs)) >> (SCALE_BS - 1);       //b1_ = -2*a_*( (a_-1) + apcs             );
							b2_ = (a_ * ((a_ + SCALE) + amcs - sraalpha)) >> SCALE_BS;   //b2_ =    a_*( (a_+1) + amcs - sraalpha );
							a0_ = (a_ + SCALE) - amcs + sraalpha;                       //a0_ =        (a_+1) - amcs + sraalpha  ;
							a1_ = ((a_ - SCALE) - apcs) << 1;                        //a1_ =    2*( (a_-1) - apcs             );
							a2_ = (a_ + SCALE) - amcs - sraalpha;                       //a2_ =        (a_+1) - amcs - sraalpha  ;
						}
						break;
					}
				}
				break;
			}
		}
		break;
		case FILT_BUTLPF:
		case FILT_BUTHPF:
		case FILT_BESLPF:
		case FILT_BESHPF:
		{
			unsigned int w2 = (unsigned int)(((unsigned long)fc << (FIXEDMATH_BS - 1)) / sf);
			int k = ftan(w2); //k = tan(fc * piOverSf);
			long k2p1;
			switch (type_)
			{
				case FILT_BUTLPF:
				{
					b2_ = ((long)k * k) >> SCALE_BS; //= k * k;
					b0_ = b2_; //b0_ = b2_;
					b1_ = b0_ << 1; //b1_ = 2 * b0_;
					a0_ = b0_ + ((SQRT2 * (long)k) >> SCALE_BS) + SCALE; //= b0_ + (SQRT2 * k) + 1;
					a1_ = (b0_ - SCALE) << 1;  //= 2 * (b0_ - 1);
					a2_ = b0_ - ((SQRT2 * (long)k) >> SCALE_BS) + SCALE; //= b0_ - (SQRT2 * k) + 1;
				}
				break;
				case FILT_BUTHPF:
				{
					k2p1 = (((long)k * k) >> SCALE_BS) + SCALE;  //k * k + 1;
					b0_ = SCALE; //= 1;
					b2_ = b0_;   //= b0_;
					b1_ = -SCALE_X2;  //= -2;
					a0_ = k2p1 + ((SQRT2 * (long)k) >> SCALE_BS); //= k2p1 + (SQRT2 * k);
					a1_ = (k2p1 - SCALE_X2) << 1; //2 * (k2p1 - 2);
					a2_ = k2p1 - ((SQRT2 * (long)k) >> SCALE_BS); //= k2p1 - (SQRT2 * k);
				}
				break;
				case FILT_BESLPF:
				{
					b2_ = (((long)k * k) >> SCALE_BS) * 3; //= 3 * k * k;
					b0_ = b2_;  //b0_ = b2_;
					b1_ = b0_ << 1;  //b1_ = 2 * b0_;
					a0_ = SCALE + (3 * (long)k) + b0_; //= 1 + 3 * k + b0_;
					a1_ = (-SCALE << 1) + b1_;  //a1_ = -2 + b1_;
					a2_ = SCALE - (3 * (long)k) + b0_;  //a2_ = 1 - 3 * k + b0_;
				}
				break;
				case FILT_BESHPF:
				{
					long k2 = ((long)k * k) >> SCALE_BS;  //= k * k;
					b2_ = SCALE_X3; //= 3;
					b0_ = b2_; //db0_ = db2_;
					b1_ = -SCALE_X6; //= -6;
					a0_ = k2 + (3 * (long)k) + SCALE_X3; //= k2 + 3 * k + 3;
					a1_ = (k2 << 1) - SCALE_X6; //= 2 * k2 - 6;
					a2_ = k2 - (3 * (long)k) + SCALE_X3; //= k2 - 3 * k + 3;
				}
				break;
			}
		}
		break;
	}
	b0_ = (b0_ << SCALE_BS) / a0_;
	b1_ = (b1_ << SCALE_BS) / a0_;
	b2_ = (b2_ << SCALE_BS) / a0_;
	a1_ = (a1_ << SCALE_BS) / a0_;
	a2_ = (a2_ << SCALE_BS) / a0_;
}

//Process the wavetable
void BiquadFilterFM::processWavetable(Wavetable& sourceWavetable)
{

	unsigned char i;
	char sampOut, maxout;
	char sampIn, multb;

	if (type_ > 0)
	{
		maxout = 0;
		for (i = 0; i < sourceWavetable.getWaveLength(); ++i)
		{
			sampOut = BiquadProcess(sourceWavetable.getSample(i));
			sourceWavetable.setSample(i,sampOut);
			if (gainAdj_ == true)
			{
				if(sampOut==-128)
				{
					sampOut = 127;
				}
				else if(sampOut < 0)
				{
					sampOut *= -1;
				}
				if (sampOut > maxout)
				{
					maxout = sampOut;
				}
			}
			if((i&0x1F)==0x1F)  //at end of every 32 samples, do events
			{
				base_->filterDoEvents();
			}
		}
		if (gainAdj_ == true)
		{
			multb = (char)(127 / maxout);
			for (i = 0; i < sourceWavetable.getWaveLength(); ++i)
			{
				sampIn = sourceWavetable.getSample(i);
				sampOut = (char)(sampIn * multb);
				sourceWavetable.setSample(i,sampOut);
			}
			base_->filterDoEvents();
		}
	}
}
char BiquadFilterFM::BiquadProcess(char inp)
{
	const unsigned char INP_SCALE = 6;
	long bi0 = inp;
	if(gainAdj_)
	{
		bi0 <<= (INP_SCALE - 4);
	}
	else
	{
		bi0 <<= INP_SCALE;
	}
	long bo0 = b0_ * bi0 + b1_ * bi1_ + b2_ * bi2_ - a1_ * bo1_ - a2_ * bo2_;
	bo0 >>= SCALE_BS;
	bo0 = constrainInt(bo0);
	bi2_ = bi1_;
	bi1_ = bi0;
	bo2_ = bo1_;
	bo1_ = bo0;
	return constrainChar((int)(bo0 >> INP_SCALE));
}
