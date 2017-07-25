/*
//AteOdyEngine.h  Odytron for Oscitron audio engine
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

#ifndef __ATEODYENGINE_H__
#define __ATEODYENGINE_H__

#include "OdyAudio.h"
#include "OdyOscillator.h"
#include "OdyNoise.h"
#include "OdyPatch.h"
#include "OdyPatchBase.h"
#include "AteOdyEngineProgmem.h"
#include "AteOdyEngineBase.h"
//#include "OdyEngineSettings.h"
#include "OdyFilter.h"
#include "OdyPitch.h"
#include "SsHelpers.h"
#include "AteOscPitch.h"
#include "QuantizePitch.h"

class AteOdyEngine : public OdyPatchBase
{
//variables
public:
	static AteOdyEngine& getInstance()
	{
		static AteOdyEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_VCO1,
		CTRL_Q,
		CTRL_VCO2,
		CTRL_MODA,
		CTRL_MODB,
		CTRL_NULL
	};
	enum Func : unsigned char
	{
		FUNC_OSC0FM,
		FUNC_OSC1FM,
		FUNC_OSC1PWM,
		FUNC_OSCLEVELFX,
		FUNC_OSCLEVEL0,
		FUNC_OSCLEVEL1,
		FUNC_FILTFM,
		FUNC_QUANT,
		FUNC_MEM,
		FUNC_FILTTYPE
	};
	enum FxSource : unsigned char
	{
		FX_NOISE,
		FX_RINGMOD
	};
	static const unsigned int WAVE_LENGTH = 256;//64;
protected:
private:
	FxSource fxSource_ = FX_NOISE;
	unsigned char fxLevel_ = 7;
	OdyPatch* patch_;
	OdyOscillator oscillator_[2];
	OdyNoise noise_;
	Func function_ = FUNC_OSC0FM;
	AteOdyEngineBase* base_ = NULL;
	OdyAudio audio_;
	OdyFilter filter_;
	QuantizePitch quantize_;
	OdyPitch pitch_[2];
	AteOscPitch cvPitch_[2];
	char modSource_[2] = {0};
//functions
public:
	void construct(AteOdyEngineBase* base);
	const OdyPatch* getPatchPtr() const { return  patch_; }
	OdyPatch* getPatchPtr() { return patch_; }
	OdyFilter& getFilter() { return filter_; }
	const OdyFilter& getFilter() const { return filter_; }
	OdyOscillator& getOsc(unsigned char oscNum) { return oscillator_[oscNum]; }
	const OdyOscillator& getOsc(unsigned char oscNum) const { return oscillator_[oscNum]; }
	AteOscPitch& getCvPitch(unsigned char index) { return cvPitch_[index]; }
	const AteOscPitch& getCvPitch(unsigned char index) const { return cvPitch_[index]; }
	QuantizePitch& getQuantize() { return quantize_; }
	const QuantizePitch& getQuantize() const { return quantize_; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(AteOdyEngine::Func new_func);
	AteOdyEngine::Func getFunction(){return function_;}
	FxSource getFxSource(){return fxSource_;}
	unsigned char getFxLevel(){return fxLevel_;}
	void setModSource(unsigned char index, char newValue){modSource_[index] = newValue;}
	char getModSource(unsigned char index){return modSource_[index];}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char anlControl_, unsigned char newValue);
protected:
private:
	AteOdyEngine(AteOdyEngineBase* base);
	AteOdyEngine() {}
	AteOdyEngine( const AteOdyEngine &c );
	~AteOdyEngine();
	AteOdyEngine& operator=( const AteOdyEngine &c );
}; //AteOdyEngine

#endif //__ATEODYENGINE_H__
