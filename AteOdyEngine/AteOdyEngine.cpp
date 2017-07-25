/*
//AteOdyEngine.cpp  Odytron for Oscitron audio engine
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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
*/

#include "AteOdyEngine.h"

// default constructor
AteOdyEngine::AteOdyEngine(AteOdyEngineBase* base)
{
	construct(base);
} //AteOdyEngine

// default destructor
AteOdyEngine::~AteOdyEngine()
{
	if(patch_!=NULL)
	{
		delete patch_;
	}
} //~AteOdyEngine

void AteOdyEngine::construct(AteOdyEngineBase* base)
{
	base_ = base;
	patch_ = new OdyPatch(this);
}

void AteOdyEngine::initialize()
{
	unsigned char i;
	
	//load vanilla
	//for(i=0;i<5;++i)
	//{
	//patchCtrlChanged(i,patch_->getCtrlValue(i));
	//}
	//for(i=0;i<10;++i)
	//{
	//patchValueChanged(i,patch_->getFunctionValue(i));
	//patchOptionChanged(i,patch_->getOptionValue(i));
	//}
	//

	cvPitch_[0].setCoarseOffset(2304);  //init val
	cvPitch_[1].setCoarseOffset(0);  
	pitch_[0].setOffset(pgm_read_byte(&(PITCH_OFFSET[128])));  //never any offset for osc1
	pitch_[1].setOffset(pgm_read_byte(&(PITCH_OFFSET[128])));  //init val
	filter_.setFc(127);


	//patch_->setFunctionValue(FUNC_OSCLEVEL0,15);
	//patch_->setFunctionValue(FUNC_OSCLEVEL1,15);
	//patch_->setFunctionValue(FUNC_OSC1PWM,15);
	//patch_->setFunctionValue(FUNC_FILTFM,15);
	//patch_->setFunctionValue(FUNC_QUANT,1);
	//patch_->setFunctionValue(FUNC_MEM,0);
	//patch_->setFunctionValue(FUNC_FILTTYPE,OdyFilter::SIMPLE);
	//patch_->setOptionValue(FUNC_OSCLEVEL1,true);
	//patch_->setOptionValue(FUNC_FILTFM,true);
	//patch_->setOptionValue(FUNC_QUANT,false);
	//patch_->setOptionValue(FUNC_MEM,false);

	audio_.initialize();

}

void AteOdyEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	char fm;
	char filt;
	unsigned int linearFreq[2];
	noise_.refresh();
	

	oscillator_[1].refresh(modSource_[0],modSource_[0]);
	
	if(filter_.getFmBSource()==OdyFilter::S_AND_H)
	{
		filt = modSource_[0];
	}
	else
	{
		filt = modSource_[1];
	}

	filter_.refresh(0,0,filt);

	for(i=0;i<2;++i)
	{
		if(quantize_.getQntScale()>0)
		{
			linearFreq[i] = quantize_.process(cvPitch_[i].getOutput());
		}
		else
		{
			linearFreq[i] = cvPitch_[i].getOutput();
		}
		if(i==1)
		{
			linearFreq[1] += linearFreq[0];
			if(linearFreq[1]>8128)  //LIN_FREQ_MAX
			{
				linearFreq[1] = 8128;
			}
		}
		pitch_[i].setInput(cvPitch_[i].calcFrequency(linearFreq[i]));
		if(pitch_[i].getFmASource()==OdyPitch::SINE)
		{
			fm = modSource_[0];
		}
		else
		{
			fm = modSource_[1];
		}
		pitch_[i].refresh(0,fm,0);
		audio_.setSampleFreq(i,(unsigned long)pitch_[i].getOutput()*(WAVE_LENGTH>>2));  //Finally scale freqs back down again (/4)
	}
}

void AteOdyEngine::setFunction(AteOdyEngine::Func newFunc)
{
	if(newFunc==FUNC_QUANT && patch_->getOptionValue(FUNC_QUANT)==true)
	{
		function_ = FUNC_MEM;
	}
	else
	{
		function_ = newFunc;
	}
	base_->engineFunctionChanged((unsigned char)function_,patch_->getFunctionValue(function_));
	base_->engineOptionChanged((unsigned char)function_,patch_->getOptionValue(function_));
}

//****************************************patch events********************************************
void AteOdyEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_OSC0FM:
		pitch_[0].setFmAAmount(newValue);
		break;
		case FUNC_OSC1FM:
		pitch_[1].setFmAAmount(newValue);
		break;
		case FUNC_OSC1PWM:
		oscillator_[1].setPwmAmout(newValue<<4);
		break;
		case FUNC_OSCLEVELFX:
		fxLevel_ = newValue>>1;
		noise_.setLevel(newValue>>1);
		break;
		case FUNC_OSCLEVEL0:
		oscillator_[0].setLevel(newValue>>1);
		break;
		case FUNC_OSCLEVEL1:
		oscillator_[1].setLevel(newValue>>1);
		break;
		case FUNC_FILTFM:
		filter_.setFmBAmount(newValue);
		break;
		case FUNC_QUANT:
		quantize_.setQntScale(newValue);
		break;
		case FUNC_MEM:
		break;
		case FUNC_FILTTYPE:
		filter_.setType((OdyFilter::FiltType)newValue);
		break;
	}
	if(func==function_ || func==FUNC_FILTTYPE)
	{
		base_->engineFunctionChanged(func,newValue);
		
	}
}

void AteOdyEngine::patchOptionChanged(unsigned char func, bool newOpt)
{
	switch (func)
	{
		case FUNC_OSC0FM:
		pitch_[0].setFmASource((OdyPitch::PitchFmASource)newOpt);
		break;
		case FUNC_OSC1FM:
		pitch_[1].setFmASource((OdyPitch::PitchFmASource)newOpt);
		break;
		case FUNC_OSC1PWM:
		audio_.setWaveSync(newOpt);
		break;
		case FUNC_OSCLEVELFX:
		fxSource_ = (FxSource)newOpt;
		break;
		case FUNC_OSCLEVEL0:
		oscillator_[0].setWaveform((OdyOscillator::OscWave)newOpt);
		break;
		case  FUNC_OSCLEVEL1:
		oscillator_[1].setWaveform((OdyOscillator::OscWave)newOpt);
		break;
		case FUNC_FILTFM:
		filter_.setFmBSource((OdyFilter::FiltFmBSource)newOpt);
		break;
		case FUNC_QUANT:
		if(newOpt==true)
		{
			patch_->setOptionValue(FUNC_MEM,true);
			setFunction(FUNC_MEM);
		}
		break;
		case FUNC_MEM:
		if(newOpt==false)
		{
			patch_->setOptionValue(FUNC_QUANT,false);
			setFunction(FUNC_QUANT);
		}
		break;
	}
	if(func==function_)
	{
		base_->engineOptionChanged(func,newOpt);
	}
}

void AteOdyEngine::patchCtrlChanged(unsigned char anlControl_, unsigned char newValue)
{
	switch (anlControl_)
	{
		case CTRL_VCO1:
		//pitch_[0].setOffset(pgm_read_byte(&(PITCH_OFFSET[128-(newValue>>1)])));
		cvPitch_[0].setCoarseOffset((unsigned int)newValue<<4);
		break;
		case CTRL_Q:
		filter_.setQ(newValue);
		break;
		case CTRL_VCO2:
		if (newValue>245)  //yuk, to deal with pots that don't go to top
		{
			pitch_[1].setOffset(pgm_read_byte(&(PITCH_OFFSET[255])));
		} 
		else
		{
			pitch_[1].setOffset(pgm_read_byte(&(PITCH_OFFSET[(newValue>>1)+128])));
		}
		break;
		case CTRL_MODA:
		modSource_[0] = newValue - 128;
		break;
		case CTRL_MODB:
		modSource_[1] = newValue - 128;
		break;
		//case  CTRL_FC:
		//filter_.setFc(newValue);
		//break;
	}
}
