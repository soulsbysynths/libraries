//MulAtmegaAtmEngine.h  Audio engine for Atmegatron
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

#ifndef __MULATMEGAATMENGINE_H__
#define __MULATMEGAATMENGINE_H__

#include "AtmOscillator.h"
#include "MulAtmegaAtmPatch.h"
#include "MulAtmegaAtmPatchBase.h"
#include "MulAtmegaAtmEngineProgmem.h"
#include "MulAtmegaAtmEngineBase.h"
#include "MulAtmegaAtmEngineSettings.h"
#include "MasterClock.h"
#include "Midi.h"
#include "MidiBase.h"
#include "Arpeggiator.h"
#include "ArpeggiatorBase.h"
#include "Wavetable.h"

#ifndef HIGH
#define HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

#define VOICES 6
#define VOICES_ALL 0

class MulAtmegaAtmEngine : public MidiBase, ArpeggiatorBase, MulAtmegaAtmPatchBase
{
	
	//variables
	public:
	static MulAtmegaAtmEngine& getInstance()
	{
		static MulAtmegaAtmEngine instance; // Guaranteed to be destroyed.
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
		FUNC_MINLENGTH,
		FUNC_QUANT,
		FUNC_PORTA,
		FUNC_BITCRUSH,
		FUNC_FMAMT
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
	enum VoiceMess : unsigned char
	{
		MESS_TRIGGER = 0,
		MESS_RELEASE,
		MESS_WAVE,
		MESS_FILTTYPE,
		MESS_FENVA,
		MESS_FENVDR,
		MESS_FENVS,
		MESS_AENVA,
		MESS_AENVD,
		MESS_AENVS,
		MESS_AENVR,
		MESS_LFOWAVE,
		MESS_LFOSPEED,
		MESS_ARPPATT,
		MESS_ARPSPEED,
		MESS_PORTA,
		MESS_CRUSH,
		MESS_MEM,
		MESS_OSCCAL = 254  //keep 255 free until I know why bad stuff happens
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
	MulAtmegaAtmPatch* patch_;
	AtmOscillator* oscillator_;

	Func function_ = FUNC_WAVE;
	unsigned char voice_ = VOICES_ALL;
	unsigned char bank_ = LOW;
	MulAtmegaAtmEngineBase* base_ = NULL;
	Arpeggiator* arpeggiator_;
	unsigned int frequency_ = 440;
	unsigned char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	unsigned char curOsc_ = 0;
	unsigned char oscNotes_[VOICES];
	unsigned char oscNoteOrder_[VOICES];
	bool mono_ = false;
	//functions
	public:
	void construct(MulAtmegaAtmEngineBase* base);
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	const MulAtmegaAtmPatch* getPatchPtr() const { return  patch_; }
	MulAtmegaAtmPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setVoice(unsigned char newVoice);
	unsigned char getVoice(){return voice_;}
	void setFunction(MulAtmegaAtmEngine::Func newFunc);
	MulAtmegaAtmEngine::Func getFunction(){return function_;}
	void setBank(unsigned char newBank);
	unsigned char getBank(){return bank_;}
	void setMono(bool newValue){mono_ = newValue;}
	bool getMono(){return mono_;}
	void patchValueChanged(unsigned char voice, unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char voice, unsigned char func, bool newOpt);
	void patchCtrlChanged(unsigned char voice,  unsigned char bank, unsigned char ctrl, unsigned char newValue);
	void midiControlChangeReceived(unsigned char cc, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void);
	void midiClockStopReceived(void);
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiPitchBendReceived(char bend);
	void arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote);
	void txOsccalib(unsigned char voice, bool up);
	protected:
	private:
	MulAtmegaAtmEngine(MulAtmegaAtmEngineBase* base);
	MulAtmegaAtmEngine() {}
	MulAtmegaAtmEngine( const MulAtmegaAtmEngine &c );
	~MulAtmegaAtmEngine();
	MulAtmegaAtmEngine& operator=( const MulAtmegaAtmEngine &c );
	void tieOptions(Func minFunc, Func maxFunc, bool newOpt);
	void tieControls(unsigned char bank, unsigned char ctrl);
	void triggerNote(unsigned char note);
	void releaseNote(unsigned char note);
	void txWave(unsigned char voice, bool bank, unsigned char table);
	void txTriggerNote(unsigned char voice, unsigned char note, bool jumpSustain);
	void txReleaseNote(unsigned char voice);
	void txIntMess(unsigned char voice, unsigned char mess, int val);
	void txUintMess(unsigned char voice, unsigned char mess, unsigned int val);
}; //MulAtmegaAtmEngine

#endif //__MULATMEGAATMENGINE_H__
