//AteOscEngine.cpp  Audio engine for Atmegatron
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

#include "AteOscEngine.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
AteOscEngine::AteOscEngine(AteOscEngineBase* base)
{
	construct(base);
} //AteOscEngine

// default destructor
AteOscEngine::~AteOscEngine()
{
	if(oscillator_!=NULL)
	{
		delete oscillator_;
	}
	if(audio_!=NULL)
	{
		delete audio_;
	}
	if(patch_!=NULL)
	{
		delete patch_;
	}
	if(flanger_!=NULL)
	{
		delete flanger_;
	}
	if(pwm_!=NULL)
	{
		delete pwm_;
	}
} //~AteOscEngine

void AteOscEngine::construct(AteOscEngineBase* base)
{
	base_ = base;
	patch_ = new AteOscPatch(this);
	oscillator_ = new AteOscOscillator(patch_->PATCH_SIZE*16);
	audio_ = new AtmAudio(128);
	audio_->resizeWavetable(waveLength_);
	flanger_ = new Flanger(128);
	flanger_->resizeWavetable(waveLength_);
	pwm_ = new Pwm(waveLength_);
}

void AteOscEngine::initialize()
{
	unsigned char i;
	
	audio_->initialize();
	//load vanilla
	for(i=0;i<CTRLS;++i)
	{
		patchCtrlChanged(i,patch_->getCtrlValue(i));
	}
	for(i=0;i<FUNCS;++i)
	{
		patchValueChanged(i,patch_->getFunctionValue(i));
		patchOptionChanged(i,patch_->getOptionValue(i));
	}
	//filter_.setEnvAmount(255);
	//filter_.setLfoAmount(255);
	filter_.setFc(127);
	filter_.setSampFreqTracking(false);
	pwm_->setEnvAmount(255);
	flanger_->setEnvAmount(255);
	setFunction(FUNC_WAVE);
}
void AteOscEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	

	if(patch_->getOptionValue(FUNC_PORTA)==true)
	{
		frequency_ = portamento_.getOutput();
	}
	//if(waveLength_==0)
	//{
		//audio_->setSampleFreq((unsigned long)frequency_ << 6);  //simplified maths
	//}
	//else
	//{
		audio_->setSampleFreq((unsigned long)frequency_ * (waveLength_>>2));
//	}
	
	//filter_.refresh(audio_->getSampleFreq(),cvFilter,cvFilter);
	filter_.refresh(audio_->getSampleFreq(),0,0);
	portamento_.refresh(ticksPassed);
	
	Wavetable working_buffer_(waveLength_);
	oscillator_->copyWavetable(working_buffer_);
	pwm_->processWavetable(working_buffer_,(char)(patch_->getCtrlValue(CTRL_PWM)>>1),0);
	if(patch_->getOptionValue(FUNC_BITCRUSH)==true)
	{
		wavecrusher_.processWavetable(working_buffer_);
	}
	filter_.processWavetable(working_buffer_);
	
	flanger_->processWavetable(working_buffer_,(char)(patch_->getCtrlValue(CTRL_FX)>>1),0);
	if(patch_->getOptionValue(FUNC_BITCRUSH)==false)
	{
		wavecrusher_.processWavetable(working_buffer_);
	}	
	audio_->pasteWavetable(working_buffer_);

}

void AteOscEngine::setFunction(AteOscEngine::Func newFunc)
{
	bool col;
	function_ = newFunc;
	base_->engineFunctionChanged((unsigned char)function_,patch_->getFunctionValue(function_),patch_->getOptionValue(function_));
}

void AteOscEngine::setFrequency(unsigned int newValue)
{
	portamento_.setInput(newValue);
	if(patch_->getOptionValue(FUNC_PORTA)==false)
	{
		frequency_ = quantize_.processPitch(newValue);
	}
	//portamento sets frequency_ in poll
}

void AteOscEngine::setWavelength(unsigned char newValue)
{
	if(newValue!=waveLength_)
	{
		waveLength_ = newValue;
		audio_->resizeWavetable(waveLength_);
		flanger_->resizeWavetable(waveLength_);
		pwm_->setWaveLength(waveLength_);
	}
}

//****************************************patch events********************************************
void AteOscEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_->setTable(newValue);
		break;
		case FUNC_WAVELEN:
		setWavelength(1<<(newValue+4));
		break;
		case FUNC_CAPFREQ:
		break;
		case FUNC_FILT:
		filter_.setType((BiquadFilter::FiltType) newValue);
		break;
		case FUNC_PORTA:
		portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		if(newValue==0)
		{
			quantize_.setQntAmount(0);
		}
		else
		{
			quantize_.setQntAmount(newValue+1);
		}
		break;
		case FUNC_BITCRUSH:
		wavecrusher_.setType(newValue);
		break;
	}
	if(func==function_)
	{
		base_->engineFunctionChanged(func,newValue,patch_->getOptionValue(func));
	}
}

void AteOscEngine::patchOptionChanged(unsigned char func, bool newOpt)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_->setBank((unsigned char)newOpt);
		break;
		case FUNC_WAVELEN:
		oscillator_->setUserMode(newOpt);
		break;
		case FUNC_FILT:
		filter_.setGainAdj(newOpt);
		break;
		case FUNC_PORTA:
		//nothing to do here
		break;
	}
}

void AteOscEngine::patchCtrlChanged(unsigned char anlControl_, unsigned char newValue)
{
	switch (anlControl_)
	{
		case CTRL_FILT:
		filter_.setFc(newValue);
		//cvFilter = newValue - 128;
		break;
		case CTRL_Q:
		filter_.setQ(newValue);
		break;
	}
}