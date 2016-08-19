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
#include "BiquadFilterFM.h"
//#include "BiquadFilter.h"
#include "Portamento.h"
#include "QuantizePitch.h"
#include "AteOscPitch.h"
#include "Wavecrusher.h"
#include "Flanger.h"
#include "Pwm.h"

class AteOscEngine : public AteOscPatchBase, BiquadFilterFMBase
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
		CTRL_PITCHFINE = 0,
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
		FUNC_PITCHCOARSE,
		FUNC_MINLENGTH,
		FUNC_FILT,
		FUNC_PORTA,
		FUNC_BITCRUSH,
		FUNC_MEM
	};
	//enum Func : unsigned char
	//{
		//FUNC_WAVE = 0,
		//FUNC_WAVELEN,
		//FUNC_PITCHCOARSE,
		//FUNC_PORTA,
		//FUNC_MINLENGTH,
		//FUNC_FILT,
		//FUNC_BITCRUSH,
		//FUNC_MEM
	//};
	protected:
	private:
	static const unsigned char FUNCS = 8;
	static const unsigned char CTRLS = 6;
	static const unsigned char LARGEST_WAVELENGTH = 128;
	AteOscPatch* patch_;
	AteOscOscillator oscillator_;
	Func function_ = FUNC_WAVE;
	AteOscEngineBase* base_ = NULL;
	AtmAudio* audio_;
	BiquadFilterFM* filter_;
	//BiquadFilter filter_;
	Portamento portamento_;
	AteOscPitch pitch_;
	AteOscPitch filterFc_;
	QuantizePitch quantize_;
	WaveCrusher wavecrusher_;
	Flanger* flanger_;
	Pwm* pwm_;
	unsigned char waveLength_ = 128;
	bool updateFilter_ = false;
	//functions
	public:
	void construct(AteOscEngineBase* base);
	const AteOscPatch* getPatchPtr() const { return  patch_; }
	AteOscPatch* getPatchPtr() { return patch_; }
	const AteOscOscillator& getOscillator() const { return  oscillator_; }
	AteOscOscillator& getOscillator() { return oscillator_; }
	AteOscPitch& getPitch() { return pitch_; }
	const AteOscPitch& getPitch() const { return pitch_; }
	QuantizePitch& getQuantize() { return quantize_; }
	const QuantizePitch& getQuantize() const { return quantize_; }
	void initialize();
	void pollPitch(unsigned char ticksPassed);
	void pollWave();
	void setFunction(AteOscEngine::Func new_func);
	AteOscEngine::Func getFunction(){return function_;}
	void setWavelength(unsigned char newValue);
	unsigned char getWavelength(){return waveLength_;}
	void setFilterFcInput(unsigned int newInputValue);
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char anlControl_, unsigned char newValue);
	void filterDoEvents();
	protected:
	private:
	AteOscEngine(AteOscEngineBase* base);
	AteOscEngine() {}
	AteOscEngine( const AteOscEngine &c );
	~AteOscEngine();
	AteOscEngine& operator=( const AteOscEngine &c );
}; //AteOscEngine

#endif //__ATEOSCENGINE_H__
