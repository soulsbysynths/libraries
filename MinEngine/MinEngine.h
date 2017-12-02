//MinEngine.h  Audio engine for miniAtmegatron  
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

#ifndef __MINENGINE_H__
#define __MINENGINE_H__

#include "AtmAudio.h"
#include "AtmOscillator.h"
#include "AtmPatch.h"
#include "AtmPatchBase.h"
#include "Envelope.h"
#include "Amplifier.h"
#include "MinEngineProgmem.h"
#include "MinEngineBase.h"
#include "MinEngineSettings.h"
#include "BiquadFilter.h"
#include "MasterClock.h"
#include "Lfo.h"
#include "Portamento.h"
#include "AtmPitch.h"
#include "Midi.h"
#include "MidiBase.h"
#include "StepSequencer.h"
#include "StepSequencerBase.h"
#include "Wavecrusher.h"
#include "Flanger.h"
#include "ClipDistortion.h"
#include "Pwm.h"


class MinEngine : public MidiBase, StepSequencerBase, AtmPatchBase
{
	//variables
	public:
	static MinEngine& getInstance()
	{
		static MinEngine instance; // Guaranteed to be destroyed.
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
		CC_PITCHLFOMOD = 1,
		CC_FX = 5,
		CC_VOLUME = 7,
		CC_FILTERENV = 16,
		CC_DISTORTION = 17,
		CC_WAVEFORM = 32,
		CC_FILTTYPE = 33,
		CC_FILTENVSHAPE = 34,
		CC_AMPENVSHAPE = 35,
		CC_SEQUENCE = 36,
		CC_FILTCUTOFF = 74,
		CC_FILTRES = 71,
		CC_PITCHLFO = 77,
		CC_LFOSHAPE = 78,
		CC_LFOCLOCKDIV = 79,
		CC_PWM = 91,
		CC_AMPLFO = 92,
		CC_FILTLFO = 93,
		CC_PITCHENV = 94,
		CC_FLANGE = 95,
		CC_ALLNOTESOFF = 123	
	};
	protected:
	private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	#define NP_CLASSIC 0
	#define NP_LOW 1
	#define NP_HIGH 2
	#define NP_LAST 3
	MasterClock masterClock_;
	Midi* midi_;
	AtmPatch* patch_;
	AtmOscillator* oscillator_;
	Func function_ = FUNC_WAVE;
	unsigned char bank_ = LOW;
	MinEngineBase* base_ = NULL;
	AtmAudio* audio_;
	Envelope ampEnvelope_;
	Envelope filtEnvelope_;
	Amplifier amplifier_;
	BiquadFilter filter_;
	Lfo lfo_;
	Portamento portamento_;
	AtmPitch pitch_;
	StepSequencer* sequencer_;
	WaveCrusher wavecrusher_;
	Flanger* flanger_;
	ClipDistortion clipdistortion_;
	Pwm* pwm_;
	unsigned int frequency_ = 440;
	char totNotesOn_ = 0;
	char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	unsigned char testNote_ = 60;
	unsigned char seqBaseNote_ = 60;
	bool noteOn_[128] = {false};
	#if NOTE_PRIORITY == NP_LOW
	unsigned char noteLowest_ = 127;
	#elif NOTE_PRIORITY == NP_HIGH
	unsigned char noteHighest_ = 0;
	#elif NOTE_PRIORITY == NP_LAST
	unsigned char noteOrder_[128] = {0};
	unsigned char noteLast_ = 0;
	#endif
	unsigned char intBpm_ = 6;
	//functions
	public:
	void construct(MinEngineBase* base);
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	const AtmPatch* getPatchPtr() const { return  patch_; }
	AtmPatch* getPatchPtr() { return patch_; }
	const AtmOscillator* getOscillatorPtr() const { return  oscillator_; }
	AtmOscillator* getOscillatorPtr() { return oscillator_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	unsigned char getTestNote() {return testNote_;}
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(MinEngine::Func newFunc);
	void setBank(unsigned char newBank);
	void setIntBpm(unsigned char newBPM);
	MinEngine::Func getFunction(){return function_;}
	unsigned char getBank(){return bank_;}
	void initPatch();
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
	void midiProgramChangeReceived(unsigned char patchNum){}
	void stepseqNoteEvent(unsigned char lastStep, unsigned char newStep);
	protected:
	private:
	MinEngine() {}
	MinEngine(MinEngineBase* base);
	~MinEngine();
	MinEngine( const MinEngine &c );
	MinEngine& operator=( const MinEngine &c );
	void triggerNote(unsigned char note);
	void releaseNote();
}; //MinEngine

#endif //__MINENGINE_H__
