//AteOscEngine.h  Audio engine for Atmegatron
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

#ifndef __ATEOSCENGINE_H__
#define __ATEOSCENGINE_H__

#include "AtmAudio.h"
#include "AteOscOscillator.h"
#include "AteOscPatch.h"
#include "AteOscPatchBase.h"
#include "AteOscEngineProgmem.h"
#include "AteOscEngineBase.h"
#include "BiquadFilter.h"
#include "Portamento.h"
#include "QuantizePitch.h"
#include "AteOscPitch.h"
#include "Wavecrusher.h"
#include "Flanger.h"
#include "Pwm.h"


class AteOscEngine : public AteOscPatchBase
{
	
	//variables
	public:
	static AteOscEngine& getInstance()
	{
		static AteOscEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_PITCHOFF = 0,
		CTRL_PWM,
		CTRL_FILTOFF,
		CTRL_Q,
		CTRL_FX,
		CTRL_SAMPLE
	};
	enum Func : unsigned char
	{
		FUNC_WAVE = 0,
		FUNC_WAVELEN,
		FUNC_CAPFREQ,
		FUNC_MINCAPLEN,
		FUNC_FILT,
		FUNC_PORTA,
		FUNC_BITCRUSH,
		FUNC_MEM
	};
	protected:
	private:
	static const unsigned char FUNCS = 8;
	static const unsigned char CTRLS = 6;
	static const unsigned char HIGH = 1;
	static const unsigned char LOW = 0;
	AteOscPatch* patch_;
	AteOscOscillator* oscillator_;
	Func function_ = FUNC_WAVE;
	AteOscEngineBase* base_ = NULL;
	AtmAudio* audio_;
	BiquadFilter filter_;
	Portamento portamento_;
	AteOscPitch pitch_;
	QuantizePitch quantize_;
	WaveCrusher wavecrusher_;
	Flanger* flanger_;
	Pwm* pwm_;
	unsigned char waveLength_ = 128;
	//functions
	public:
	void construct(AteOscEngineBase* base);
	const AteOscPatch* getPatchPtr() const { return  patch_; }
	AteOscPatch* getPatchPtr() { return patch_; }
	const AteOscOscillator* getOscillatorPtr() const { return  oscillator_; }
	AteOscOscillator* getOscillatorPtr() { return oscillator_; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(AteOscEngine::Func new_func);
	AteOscEngine::Func getFunction(){return function_;}
	void setFrequency(unsigned int newCvValue);
	void setFiltFc(unsigned int newCvValue);
	void setWavelength(unsigned char newValue);
	unsigned char getWavelength(){return waveLength_;}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char anlControl_, unsigned char newValue);
	protected:
	private:
	AteOscEngine(AteOscEngineBase* base);
	AteOscEngine() {}
	AteOscEngine( const AteOscEngine &c );
	~AteOscEngine();
	AteOscEngine& operator=( const AteOscEngine &c );
	

}; //AteOscEngine

#endif //__ATEOSCENGINE_H__
