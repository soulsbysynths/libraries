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
	audio_ = new AtmAudio(LARGEST_WAVELENGTH);
	audio_->resizeWavetable(waveLength_);
	flanger_ = new Flanger(LARGEST_WAVELENGTH);
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
	pitch_.setInput(110<<1);
	pwm_->setEnvAmount(255);
	flanger_->setEnvAmount(255);
	filter_.setAbsoluteFc(true);
	setFunction(FUNC_WAVE);
}
void AteOscEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	unsigned long sf = 0;
	//pitch_.setInput(440<<2);
	if(patch_->getOptionValue(FUNC_PORTA)==true)
	{
		portamento_.setInput(pitch_.getFrequency());
		portamento_.refresh(ticksPassed);
		sf = (unsigned long)portamento_.getOutput() * (waveLength_>>2);
	}
	else
	{
		sf = (unsigned long)quantize_.processPitch(pitch_.getFrequency()) * (waveLength_>>2);
	}
	if(sf!=audio_->getSampleFreq())
	{
		audio_->setSampleFreq(sf);
		//filter_.refresh(sf);
		filter_.refresh(sf,NULL,NULL);
	}


	Wavetable working_buffer_(waveLength_);
	oscillator_.copyWavetable(working_buffer_);
	pwm_->processWavetable(working_buffer_,(char)(patch_->getCtrlValue(CTRL_PWM)>>1),NULL);
	if(patch_->getOptionValue(FUNC_BITCRUSH)==true)
	{
		wavecrusher_.processWavetable(working_buffer_);
	}
	
	filter_.processWavetable(working_buffer_);
	if(patch_->getCtrlValue(CTRL_FX)>0)  //not nice as flanger buffer isn't updated when 0
	{
		flanger_->processWavetable(working_buffer_,(char)(patch_->getCtrlValue(CTRL_FX)>>1),NULL);
	}
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
	base_->engineFunctionChanged((unsigned char)function_,patch_->getFunctionValue(function_));
	base_->engineOptionChanged((unsigned char)function_,patch_->getOptionValue(function_));
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

void AteOscEngine::setFilterFcInput(unsigned int newInputValue)
{
	filterFc_.setInput(newInputValue);
	//filter_.setFc(filterFc_.getOutput()>>1);  //twice range of pitch because sf = 2 * f
	filter_.setFcAbs(filterFc_.getOutput()>>1);  //twice range of pitch because sf = 2 * f
	//filter_.refresh(audio_->getSampleFreq());
	filter_.refresh(audio_->getSampleFreq(),NULL,NULL);
}

//****************************************patch events********************************************
void AteOscEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_.setTable(newValue);
		break;
		case FUNC_WAVELEN:
		setWavelength(1<<(newValue+4));
		break;
		case FUNC_MINLENGTH:
		base_->engineMinLengthChanged(pgm_read_byte(&(AUDIO_MIN_LENGTH[newValue])));
		break;
		case FUNC_PITCHCOARSE:
		pitch_.setCoarseOffset((unsigned int)newValue<<8);
		break;
		case FUNC_FILT:
		//filter_.setType((BiquadFilterFM::FiltType) newValue);
		filter_.setType((BiquadFilter::FiltType) newValue);
		//filter_.refresh(audio_->getSampleFreq());
		filter_.refresh(audio_->getSampleFreq(),NULL,NULL);
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
		base_->engineFunctionChanged(func,newValue);
	}
}

void AteOscEngine::patchOptionChanged(unsigned char func, bool newOpt)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_.setBank((unsigned char)newOpt);
		break;
		case FUNC_PITCHCOARSE:
		pitch_.setTopHalf(newOpt);
		break;
		case FUNC_FILT:
		filter_.setGainAdj(newOpt);
		filter_.refresh(audio_->getSampleFreq(),NULL,NULL);
		break;
		case FUNC_PORTA:
		//nothing to do here
		break;
	}
	if(func==function_)
	{
		base_->engineOptionChanged(func,newOpt);
	}
}

void AteOscEngine::patchCtrlChanged(unsigned char anlControl_, unsigned char newValue)
{
	unsigned char mod;
	switch (anlControl_)
	{
		case CTRL_PITCHFINE:
		pitch_.setFineOffset(newValue);
		break;
		case CTRL_FILTOFF:
		filterFc_.setCoarseOffset((unsigned int)newValue<<5);
		//filter_.setFc(filterFc_.getOutput()>>1);
		filter_.setFcAbs(filterFc_.getOutput()>>1);
		//filter_.refresh(audio_->getSampleFreq());
		filter_.refresh(audio_->getSampleFreq(),NULL,NULL);
		break;
		case CTRL_Q:
		filter_.setQ(newValue);
		//filter_.refresh(audio_->getSampleFreq());
		filter_.refresh(audio_->getSampleFreq(),NULL,NULL);
		break;
	}
}