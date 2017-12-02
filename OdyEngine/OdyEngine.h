//OdyEngine.h  Odytron audio engine singleton class
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


#ifndef __ODYENGINE_H__
#define __ODYENGINE_H__

#include "OdyAudio.h"
#include "OdyOscillator.h"
#include "OdyNoise.h"
#include "OdyPatch.h"
#include "OdyPatchBase.h"
#include "Envelope.h"
#include "OdyAmplifier.h"
#include "OdyEngineProgmem.h"
#include "OdyEngineBase.h"
//#include "OdyEngineSettings.h"
#include "OdyFilter.h"
#include "OdyHpf.h"
#include "OdyLfo.h"
#include "Portamento.h"
#include "OdyPitch.h"
#include "Midi.h"
#include "MidiBase.h"
#include "SsHelpers.h"



class OdyEngine : public MidiBase, OdyPatchBase
{
	//variables
	public:
	static OdyEngine& getInstance()
	{
		static OdyEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_VCO1,
		CTRL_VCO2,
		CTRL_LFO,
		CTRL_FC,
		CTRL_Q,
		CTRL_HPF
	};
	enum Func : unsigned char
	{
		FUNC_OSC0FMA,
		FUNC_OSC0FMB,
		FUNC_OSC1FMA,
		FUNC_OSC1FMB,
		FUNC_OSC1PW,
		FUNC_OSC1PWM,
		FUNC_OSCLEVELFX,
		FUNC_OSCLEVEL0,
		FUNC_OSCLEVEL1,
		FUNC_FILTFMA,
		FUNC_FILTFMB,
		FUNC_ENVR,
		FUNC_ENVS,
		FUNC_ENVD,
		FUNC_ENVA,
		FUNC_PORTAMENTO,
		FUNC_ENVR2,
		FUNC_ENVA2,
		FUNC_MEM,
		FUNC_FILTTYPE
	};
	enum MidiCC : unsigned char
	{
		CC_MODWHEEL = 1,
		CC_PORTAMENTO = 5,
		CC_FILTFMB = 16,
		CC_FILTFMBSOURCE = 17,
		CC_OSC1FMA = 20,
		CC_OSC1FMB = 21,
		CC_OSC2FMA = 22,
		CC_OSC2FMB = 23,
		CC_MIXFX = 24,
		CC_MIXOSC1 = 25,
		CC_MIXOSC2 = 26,
		CC_WAVEFX = 30,
		CC_WAVEOSC1 = 31,
		CC_WAVEOSC2 = 32,
		CC_FILTTYPE = 33,
		CC_OSC1FMASOURCE = 34,
		CC_OSC1FMBSOURCE = 35,
		CC_OSC2FMASOURCE = 36,
		CC_OSC2FMBSOURCE = 37,
		CC_SYNC = 38,
		CC_PWMSOURCE = 39,
		CC_KBRDTRACKING = 40,
		CC_DCASOURCE = 41,
		CC_FILTRES = 71,
		CC_AMPENVR = 72,
		CC_AMPENVA = 73,
		CC_FILTCUTOFF = 74,
		CC_AMPENVD = 75,
		CC_FILTFMA = 77,
		CC_FILTFMASOURCE = 78,
		CC_LFOSPEED = 79,
		CC_AMPENVS = 80,
		CC_FENVA = 81,
		CC_FENVDR = 82,
		CC_PULSEWIDTH = 91,
		CC_PWM = 92,
		CC_OSC1PITCH = 93,
		CC_OSC2PITCH = 94,
		CC_HPF = 95,
		CC_ALLNOTESOFF = 123
	};
	enum FxSource : unsigned char
	{
		FX_NOISE,
		FX_RINGMOD
	};
	static const unsigned int WAVE_LENGTH = 256;//64;
	protected:
	private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	
	
	FxSource fxSource_ = FX_NOISE;
	unsigned char fxLevel_ = 7;
	
	//MasterClock masterClock_;
	Midi* midi_;
	OdyPatch* patch_;
	OdyOscillator oscillator_[2];
	OdyNoise noise_;
	Func function_ = FUNC_OSC0FMA;
	OdyEngineBase* base_ = NULL;
	OdyAudio audio_;
	Envelope adsrEnvelope_;
	Envelope arEnvelope_;
	OdyAmplifier amplifier_;
	OdyFilter filter_;
	OdyHpf hpf_;
	OdyLfo lfo_;
	Portamento portamento_[2];
	OdyPitch pitch_[2];
	unsigned char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	unsigned char noteLowest_ = 127;
	unsigned char noteHighest_ = 0;

	//functions
	public:
	void construct(OdyEngineBase* base);
	const OdyPatch* getPatchPtr() const { return  patch_; }
	OdyPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	OdyFilter& getFilter() { return filter_; }
	const OdyFilter& getFilter() const { return filter_; }
	OdyHpf& getHPF() { return hpf_; }
	const OdyHpf& getHPF() const { return hpf_; }
	OdyAmplifier& getAmp() { return amplifier_; }
	const OdyAmplifier& getAmp() const { return amplifier_; }
	OdyOscillator& getOsc(unsigned char oscNum) { return oscillator_[oscNum]; }
	const OdyOscillator& getOsc(unsigned char oscNum) const { return oscillator_[oscNum]; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(OdyEngine::Func new_func);
	OdyEngine::Func getFunction(){return function_;}
	void setBank(unsigned char newBank);
	FxSource getFxSource(){return fxSource_;}
	unsigned char getFxLevel(){return fxLevel_;}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char anlControl_, unsigned char newValue);
	void midiControlChangeReceived(unsigned char anlControl_, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void){}
	void midiClockStopReceived(void){}
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiChannelChanged(unsigned char channel);
	void midiPitchBendReceived(char bend);
	void midiProgramChangeReceived(unsigned char patchNum);
	protected:
	private:
	OdyEngine(OdyEngineBase* base);
	OdyEngine() {}
	OdyEngine( const OdyEngine &c );
	~OdyEngine();
	OdyEngine& operator=( const OdyEngine &c );
	void triggerNote(unsigned char note);
	void releaseNote();
	
}; //OdyEngine

#endif //__ODYENGINE_H__
