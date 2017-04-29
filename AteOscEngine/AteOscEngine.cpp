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
	if(filter_!=NULL)
	{
		delete filter_;
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
	filter_ = new BiquadFilterFM(this);
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
	
	////load vanilla
	//for(i=0;i<CTRLS;++i)
	//{
		//patchCtrlChanged(i,patch_->getCtrlValue(i));
	//}
	//for(i=0;i<FUNCS;++i)
	//{
		//patchValueChanged(i,patch_->getFunctionValue(i));
		//patchOptionChanged(i,patch_->getOptionValue(i));
	//}
	//pitch_.setInput(110<<1);


	pwm_->setEnvAmount(255);
	flanger_->setEnvAmount(255);
	//portamento_.setProportional(true);
	setFunction(FUNC_WAVE);
}
void AteOscEngine::pollPitch(unsigned char ticksPassed)
{
	unsigned long sf = 0;
	//pitch_.setInput(440<<2);
	if(portaMode_)
	{
		//portamento_.setInput(pitch_.calcFrequency(pitch_.getOutput()));
		//portamento_.refresh(ticksPassed);
		//scaledPitch_ = portamento_.getOutput();
		portamento_.setInput(quantize_.process(pitch_.getOutput()));
		portamento_.refresh(ticksPassed);
		linearFreq_ = portamento_.getOutput();
	}
	else if(quantize_.getQntScale()>0)
	{
		linearFreq_ = quantize_.process(pitch_.getOutput());
	}
	else
	{
		linearFreq_ = pitch_.getOutput();
	}
	freqScaled_ = pitch_.calcFrequency (linearFreq_);
	sf = (unsigned long)freqScaled_ * (waveLength_ >> 2);
	if(sf!=audio_->getSampleFreq())
	{
		audio_->setSampleFreq(sf);
		updateFilter_ = true;
	}
}


void AteOscEngine::pollWave()
{
	Wavetable workingBuffer(waveLength_);
	oscillator_.copyWavetable(workingBuffer);
	pwm_->processWavetable(workingBuffer,pwmValue_,NULL);
	if(patch_->getOptionValue(FUNC_BITCRUSH)==true)
	{
		wavecrusher_.processWavetable(workingBuffer);
	}
	base_->engineDoEvents();
	if(updateFilter_)
	{
		updateFilter_ = false;
		int fc = (int)linearFreq_ + (int)filterFc_.getOutput() - (int)(LIN_FREQS_PER_OCT<<2);
		if(fc<0)
		{
			fc = 0;
		}
		if(fc>LIN_FREQ_MAX)
		{
			fc = LIN_FREQ_MAX;
		}
		filter_->setFc(filterFc_.calcFrequency((unsigned int)fc));  
		filter_->refresh(audio_->getSampleFreq()); 
	}
	filter_->processWavetable(workingBuffer, freqScaled_ >> 2);  //does events inside function
	if(patch_->getCtrlValue(CTRL_FX)>0)  //not nice as flanger buffer isn't updated when 0
	{
		flanger_->processWavetable(workingBuffer,flangeValue_,NULL);
	}
	if(patch_->getOptionValue(FUNC_BITCRUSH)==false)
	{
		wavecrusher_.processWavetable(workingBuffer);
	}
	audio_->pasteWavetable(workingBuffer);
	base_->engineDoEvents();
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
	//filter_->setFc(filterFc_.getOutput()>>1);  //twice range of pitch because sf = 2 * f
	updateFilter_ = true;
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
		newValue &= 0x03;  //safety against misc EEPROM data in patch
		setWavelength(1<<(newValue+4));
		break;
		case FUNC_MINLENGTH:
		newValue &= 0x07;  //safety against misc EEPROM data in patch
		base_->engineMinLengthChanged(pgm_read_byte(&(AUDIO_MIN_LENGTH[newValue])));
		break;
		case FUNC_PITCHCOARSE:
		pitch_.setCoarseOffset((unsigned int)newValue<<8);
		break;
		case FUNC_FILT:
		filter_->setType((BiquadFilterFM::FiltType) newValue);
		updateFilter_ = true;
		break;
		case FUNC_PORTA:
		if(patch_->getOptionValue(FUNC_PORTA)==true)
		{
			portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		}
		else
		{
			quantize_.setQntScale(newValue);
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
		filter_->setGainAdj(newOpt);
		updateFilter_ = true;
		break;
		case FUNC_PORTA:
		portaMode_ = newOpt;
		if(portaMode_==true)
		{
			quantize_.setQntScale(1);
			portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[patch_->getFunctionValue(FUNC_PORTA)])));
		}
		else
		{
			quantize_.setQntScale(patch_->getFunctionValue(FUNC_PORTA));
			portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[0])));
		}
		break;
		case FUNC_MINLENGTH:
		base_->engineStartCapture(newOpt);
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
		case CTRL_PWM:
		pwmValue_ = (char)(newValue>>1);
		break;
		case CTRL_FILTOFF:
		filterFc_.setCoarseOffset((unsigned int)newValue<<5);
		updateFilter_ = true;
		break;
		case CTRL_Q:
		filter_->setQ(newValue);
		updateFilter_ = true;
		break;
		case CTRL_FX:
		flangeValue_ = (char)(newValue>>1);
		break;
	}
}
//*******************filter events*************************
void AteOscEngine::filterDoEvents()
{
	base_->engineDoEvents();
}