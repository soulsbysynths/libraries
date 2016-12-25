//MinOdyEngine.h  Odytron audio engine singleton class
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
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


#ifndef __MINODYENGINE_H__
#define __MINODYENGINE_H__

#include "OdyAudio.h"
#include "OdyOscillator.h"
#include "OdyNoise.h"
#include "AtmPatch.h"
#include "AtmPatchBase.h"
#include "Envelope.h"
#include "OdyAmplifier.h"
#include "MinOdyEngineProgmem.h"
#include "MinOdyEngineBase.h"
//#include "MinOdyEngineSettings.h"
#include "OdyFilter.h"
#include "OdyHpf.h"
#include "OdyLfo.h"
#include "Portamento.h"
#include "OdyPitch.h"
#include "Midi.h"
#include "MidiBase.h"
#include "SsHelpers.h"
#include "StepSequencer.h"
#include "MasterClock.h"

class MinOdyEngine : public MidiBase, StepSequencerBase, AtmPatchBase
{
//variables
public:
	static MinOdyEngine& getInstance()
	{
		static MinOdyEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_VOLUME,
		CTRL_FILT,
		CTRL_ENV,
		CTRL_LFO,
		CTRL_AMP,
		CTRL_FX
	};
	enum Func : unsigned char
	{
		FUNC_WAVE,
		FUNC_FILT,
		FUNC_FENV,
		FUNC_AENV,
		FUNC_LFOTYPE,
		FUNC_LFOSPEED,
		FUNC_CRUSHPORTA,
		FUNC_PATTERN,
		FUNC_BPM
	};
	enum MidiCC : unsigned char
	{
		CC_PITCHLFO = 1,
		CC_FILTERENV = 16,
		CC_DISTORTION = 17,
		CC_FILTCUTOFF = 74,
		CC_FILTRES = 71,
		CC_LFOCLOCKDIV = 79,
		CC_PWM = 91,
		CC_AMPLFO = 92,
		CC_FILTLFO = 93,
		CC_PITCHENV = 94,
		CC_FLANGE = 95,
		CC_ALLNOTESOFF = 123
	};
	enum FxSource : unsigned char
	{
		FX_NOISE,
		FX_RINGMOD
	};
	static const unsigned int WAVE_LENGTH = 256;//64;
	static const unsigned char MIDI_CHANNEL = 0;
	static const unsigned char PATCH_NUM = 0;
protected:
private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	
	FxSource fxSource_ = FX_NOISE;
	unsigned char fxLevel_ = 7;
	
	//MasterClock masterClock_;
	Midi* midi_;
	AtmPatch* patch_;
	OdyOscillator oscillator_[2];
	OdyNoise noise_;
	Func function_ = FUNC_WAVE;
	unsigned char bank_ = LOW;
	MinOdyEngineBase* base_ = NULL;
	OdyAudio audio_;
	Envelope adsrEnvelope_;
	Envelope arEnvelope_;
	OdyAmplifier amplifier_;
	OdyFilter filter_;
	OdyHpf hpf_;
	OdyLfo lfo_;
	Portamento portamento_[2];
	OdyPitch pitch_[2];
	MasterClock masterClock_;
	StepSequencer* sequencer_;
	unsigned char testNote_ = 60;
	unsigned char seqBaseNote_ = 60;
	unsigned char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	unsigned char noteLowest_ = 127;
	unsigned char noteHighest_ = 0;
	unsigned char intBpm_ = 6;
	//functions
	public:
	void construct(MinOdyEngineBase* base);
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	const AtmPatch* getPatchPtr() const { return  patch_; }
	AtmPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	unsigned char getTestNote() {return testNote_;}
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
	void initPatch();
	void poll(unsigned char ticksPassed);
	void setFunction(MinOdyEngine::Func new_func);
	MinOdyEngine::Func getFunction(){return function_;}
	void setBank(unsigned char newBank);
	unsigned char getBank(){return bank_;}
	FxSource getFxSource(){return fxSource_;}
	unsigned char getFxLevel(){return fxLevel_;}
	void setIntBpm(unsigned char newBPM);
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt){}
	void midiControlChangeReceived(unsigned char ctrl, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void);
	void midiClockStopReceived(void);
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiChannelChanged(unsigned char channel){}
	void midiPitchBendReceived(char bend);
	void stepseqNoteEvent(unsigned char lastStep, unsigned char newStep);
	protected:
	private:
	MinOdyEngine(MinOdyEngineBase* base);
	MinOdyEngine() {}
	MinOdyEngine( const MinOdyEngine &c );
	~MinOdyEngine();
	MinOdyEngine& operator=( const MinOdyEngine &c );
	void triggerNote(unsigned char note);
	void releaseNote();
	
}; //MinOdyEngine

#endif //__MINODYENGINE_H__
