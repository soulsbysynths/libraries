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
	filter_.setEnvAmount(255);
	filter_.setLfoAmount(255);
	filter_.setFc(10);
	pitch_.setInput(110<<1);
	//filter_.setSampFreqTracking(false);
	pwm_->setEnvAmount(255);
	flanger_->setEnvAmount(255);
	setFunction(FUNC_WAVE);
}
void AteOscEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	
	pitch_.refresh(cvPitch);
	if(patch_->getOptionValue(FUNC_PORTA)==true)
	{
		portamento_.setInput(pitch_.getOutput());
		portamento_.refresh(ticksPassed);
		audio_->setSampleFreq((unsigned long)portamento_.getOutput() * (waveLength_>>2));
	}
	else
	{
		audio_->setSampleFreq((unsigned long)quantize_.processPitch(pitch_.getOutput()) * (waveLength_>>2));
	}
	
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
	pitch_.setInput(newValue);
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
	bool forceEvent = false;
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
		case FUNC_MINCAPLEN:
		forceEvent = true;
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
	if(func==function_  || forceEvent==true)
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
	char mod;
	switch (anlControl_)
	{
		case CTRL_FILT:
		//filter_.setFc(newValue);
		//cvFilter = newValue - 128;
		mod = newValue>>1;
		filter_.refresh(audio_->getSampleFreq(),mod,mod);
		break;
		case CTRL_Q:
		filter_.setQ(newValue);
		break;
	}
}