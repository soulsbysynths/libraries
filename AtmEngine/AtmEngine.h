//AtmEngine.h  Audio engine for Atmegatron
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

#ifndef __ATMENGINE_H__
#define __ATMENGINE_H__

#include "AtmAudio.h"
#include "AtmOscillator.h"
#include "AtmPatch.h"
#include "AtmPatchBase.h"
#include "Envelope.h"
#include "Amplifier.h"
#include "AtmEngineProgmem.h"
#include "AtmEngineBase.h"
#include "AtmEngineSettings.h"
#include "BiquadFilter.h"
#include "MasterClock.h"
#include "Lfo.h"
#include "Portamento.h"
#include "AtmPitch.h"
#include "Midi.h"
#include "MidiBase.h"
#include "Arpeggiator.h"
#include "ArpeggiatorBase.h"
#include "Wavecrusher.h"
#include "Flanger.h"
#include "ClipDistortion.h"
#include "Pwm.h"
#ifndef HIGH
#define HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

class AtmEngine : public MidiBase, ArpeggiatorBase, AtmPatchBase
{
	
	//variables
	public:
	static AtmEngine& getInstance()
	{
		static AtmEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_FILT,
		CTRL_Q,
		CTRL_ENV,
		CTRL_LFO,
		CTRL_AMP,
		CTRL_FX
	};
	enum Func : unsigned char
	{
		FUNC_WAVE,
		FUNC_FILT,
		FUNC_FENVA,
		FUNC_FENVDR,
		FUNC_FENVS,
		FUNC_AENVA,
		FUNC_AENVD,
		FUNC_AENVS,
		FUNC_AENVR,
		FUNC_LFOTYPE,
		FUNC_LFOSPEED,
		FUNC_ARPTYPE,
		FUNC_ARPSPEED,
		FUNC_PORTA,
		FUNC_BITCRUSH,
		FUNC_MEM
	};
	enum MidiCC : unsigned char
	{
		CC_PITCHLFO = 1,
		CC_PORTAMENTO = 5,
		CC_FILTERENV = 16,
		CC_DISTORTION = 17,
		CC_FILTCUTOFF = 74,
		CC_AMPENVR = 72,
		CC_AMPENVA = 73,
		CC_FILTRES = 71,
		CC_AMPENVD = 75,
		CC_LFOCLOCKDIV = 79,
		CC_PWM = 91,
		CC_AMPLFO = 92,
		CC_FILTLFO = 93,
		CC_PITCHENV = 94,
		CC_FLANGE = 95,
		CC_ALLNOTESOFF = 123
	};
	enum MidiSysex : unsigned char
	{
		SYSEX_PATCH = 0,
		SYSEX_WAVE = 1,
		SYSEX_MEM = 2,
		SYSEX_CALLPATCH = 3,
		SYSEX_CALLWAVE = 4
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
	AtmEngineBase* base_ = NULL;
	AtmAudio* audio_;
	Envelope ampEnvelope_;
	Envelope filtEnvelope_;
	Amplifier amplifier_;
	BiquadFilter filter_;
	Lfo lfo_;
	Portamento portamento_;
	AtmPitch pitch_;
	Arpeggiator* arpeggiator_;
	WaveCrusher wavecrusher_;
	Flanger* flanger_;
	ClipDistortion clipdistortion_;
	Pwm* pwm_;
	unsigned int frequency_ = 440;
	unsigned char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	#if NOTE_PRIORITY == NP_LOW
	unsigned char noteLowest_ = 127;
	#elif NOTE_PRIORITY == NP_HIGH
	unsigned char noteHighest_ = 0;
	#elif NOTE_PRIORITY == NP_LAST
	unsigned char noteOrder_[128] = {0};
	unsigned char noteLast_ = 0;
	#endif
	unsigned char sysexMesstype_ = SYSEX_PATCH;
	bool sysexMemdumping_ = false;
	unsigned char sysexMemdumpnum_ = 0;
	bool sysexComplete_ = false;
	//functions
	public:
	void construct(AtmEngineBase* base);
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	const AtmPatch* getPatchPtr() const { return  patch_; }
	AtmPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(AtmEngine::Func new_func);
	void setBank(unsigned char newBank);
	AtmEngine::Func getFunction(){return function_;}
	unsigned char getBank(){return bank_;}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue);
	void midiControlChangeReceived(unsigned char cc, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void);
	void midiClockStopReceived(void);
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data);
	void midiSysexStopReceived(void);
	void midiSysexWrite(unsigned char data);
	void midiPitchBendReceived(char bend);
	void refreshSysex();
	void writeSysexPatch(unsigned char patchNum);
	void writeSysexMemDump();
	void arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote);
	protected:
	private:
	AtmEngine(AtmEngineBase* base);
	AtmEngine() {}
	AtmEngine( const AtmEngine &c );
	~AtmEngine();
	AtmEngine& operator=( const AtmEngine &c );
	void tieOptions(Func minFunc, Func maxFunc, bool newOpt);
	void tieControls(unsigned char bank, unsigned char ctrl);
	void triggerNote(unsigned char note);
	void releaseNote();
	void writeSysexUserWave(unsigned char waveNum);
	void writeSysexCallPatch(unsigned char patchNum);
	void writeSysexCallUserWave(unsigned char waveNum);
	
}; //AtmEngine

#endif //__ATMENGINE_H__
